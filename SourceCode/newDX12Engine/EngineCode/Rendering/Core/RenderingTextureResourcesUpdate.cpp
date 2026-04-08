#include "RenderingTextureResourcesUpdate.h"

#include "../DirectX12/d3dx12.h"

#include <cwctype>
#include <vector>
#include <wincodec.h>

const wchar_t DDS[] = L".dds";
const wchar_t PNG[] = L".png";
const wchar_t JPG[] = L".jpg";
const wchar_t JPEG[] = L".jpeg";
const wchar_t Asset[] = L"/Asset/";
const wchar_t Project[] = L"/Project/";

static std::wstring GetLowerExtension(const std::wstring& InPath)
{
	size_t DotPos = InPath.find_last_of(L'.');
	if (DotPos == std::wstring::npos)
	{
		return std::wstring();
	}

	std::wstring Extension = InPath.substr(DotPos);
	for (auto& Ch : Extension)
	{
		Ch = (wchar_t)towlower(Ch);
	}

	return Extension;
}

static bool IsDDSFile(const wchar_t* Filename)
{
	return Filename && GetLowerExtension(Filename) == DDS;
}

static bool IsWICTextureFile(const wchar_t* Filename)
{
	if (!Filename)
	{
		return false;
	}

	const std::wstring Extension = GetLowerExtension(Filename);
	return Extension == PNG || Extension == JPG || Extension == JPEG;
}

static bool IsSupportedTextureFile(const wchar_t* Filename)
{
	return IsDDSFile(Filename) || IsWICTextureFile(Filename);
}

static void BuildTextureAssetPaths(FRenderingTexture& InTexture)
{
	wchar_t Filename[1024] = { 0 };
	get_path_clean_filename_w(Filename, InTexture.Filename.c_str());

	const std::wstring Extension = GetLowerExtension(Filename);
	std::wstring TextureName = Filename;
	if (!Extension.empty() && TextureName.size() >= Extension.size())
	{
		TextureName = TextureName.substr(0, TextureName.size() - Extension.size());
	}

	InTexture.Name = TextureName;

	wchar_t AssetFilenameBuff[1024] = { 0 };
	{
		wchar_t AssetFilenameBuff1[1024] = { 0 };
		wchar_t* AssetFilenamePtr = const_cast<wchar_t*>(InTexture.Filename.c_str());

		int Pos = wfind_string(AssetFilenamePtr, Asset);
		if (Pos != -1)
		{
			wchar_t* Value = wstring_mid(AssetFilenamePtr, AssetFilenameBuff1, Pos, wcslen(AssetFilenamePtr));
			wreplace_string_inline(Value, Asset, Project);
			if (!Extension.empty())
			{
				wreplace_string_inline(Value, Extension.c_str(), (L"." + InTexture.Name).c_str());
			}

			wcsncpy_s(AssetFilenameBuff, Value, _TRUNCATE);
		}
	}
	InTexture.AssetFilename = AssetFilenameBuff;

	wchar_t SimpleAssetFilenameBuff[1024] = { 0 };
	{
		wchar_t SimpleAssetFilenameBuff1[1024] = { 0 };
		wchar_t* AssetFilenamePtr = const_cast<wchar_t*>(InTexture.Filename.c_str());

		int Pos = wfind_string(AssetFilenamePtr, Asset);
		if (Pos != -1)
		{
			wchar_t* Value = wstring_mid(AssetFilenamePtr, SimpleAssetFilenameBuff1, Pos + 1, wcslen(AssetFilenamePtr) - (Pos + 1));
			if (!Extension.empty())
			{
				wreplace_string_inline(Value, Extension.c_str(), (L"." + InTexture.Name).c_str());
			}

			wcsncpy_s(SimpleAssetFilenameBuff, Value, _TRUNCATE);
		}
	}
	InTexture.SimpleAssetFilename = SimpleAssetFilenameBuff;
}

static HRESULT CreateWICTextureFromFile12(
	ID3D12Device* InDevice,
	ID3D12GraphicsCommandList* InCommandList,
	const wchar_t* InFilename,
	ComPtr<ID3D12Resource>& OutTexture,
	ComPtr<ID3D12Resource>& OutUploadBuffer)
{
	if (InDevice == nullptr || InCommandList == nullptr || InFilename == nullptr)
	{
		return E_INVALIDARG;
	}

	::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	Microsoft::WRL::ComPtr<IWICImagingFactory> ImagingFactory;
	HRESULT HR = CoCreateInstance(
		CLSID_WICImagingFactory2,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(ImagingFactory.GetAddressOf()));
	if (FAILED(HR))
	{
		HR = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(ImagingFactory.GetAddressOf()));
	}
	if (FAILED(HR))
	{
		return HR;
	}

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> Decoder;
	HR = ImagingFactory->CreateDecoderFromFilename(
		InFilename,
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		Decoder.GetAddressOf());
	if (FAILED(HR))
	{
		return HR;
	}

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> Frame;
	HR = Decoder->GetFrame(0, Frame.GetAddressOf());
	if (FAILED(HR))
	{
		return HR;
	}

	Microsoft::WRL::ComPtr<IWICFormatConverter> FormatConverter;
	HR = ImagingFactory->CreateFormatConverter(FormatConverter.GetAddressOf());
	if (FAILED(HR))
	{
		return HR;
	}

	HR = FormatConverter->Initialize(
		Frame.Get(),
		GUID_WICPixelFormat32bppRGBA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0f,
		WICBitmapPaletteTypeCustom);
	if (FAILED(HR))
	{
		return HR;
	}

	UINT Width = 0;
	UINT Height = 0;
	HR = FormatConverter->GetSize(&Width, &Height);
	if (FAILED(HR) || Width == 0 || Height == 0)
	{
		return FAILED(HR) ? HR : E_FAIL;
	}

	const UINT BytesPerPixel = 4;
	const UINT RowPitch = Width * BytesPerPixel;
	const UINT ImageSize = RowPitch * Height;

	std::vector<uint8_t> ImageData(ImageSize);
	HR = FormatConverter->CopyPixels(nullptr, RowPitch, ImageSize, ImageData.data());
	if (FAILED(HR))
	{
		return HR;
	}

	CD3DX12_RESOURCE_DESC TextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		Width,
		Height,
		1,
		1);

	CD3DX12_HEAP_PROPERTIES DefaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
	HR = InDevice->CreateCommittedResource(
		&DefaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&TextureDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(OutTexture.GetAddressOf()));
	if (FAILED(HR))
	{
		return HR;
	}

	const UINT64 UploadBufferSize = GetRequiredIntermediateSize(OutTexture.Get(), 0, 1);
	CD3DX12_HEAP_PROPERTIES UploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC UploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(UploadBufferSize);
	HR = InDevice->CreateCommittedResource(
		&UploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&UploadBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(OutUploadBuffer.GetAddressOf()));
	if (FAILED(HR))
	{
		return HR;
	}

	D3D12_SUBRESOURCE_DATA SubresourceData = {};
	SubresourceData.pData = ImageData.data();
	SubresourceData.RowPitch = RowPitch;
	SubresourceData.SlicePitch = ImageSize;

	CD3DX12_RESOURCE_BARRIER ToCopyDestBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		OutTexture.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);
	InCommandList->ResourceBarrier(1, &ToCopyDestBarrier);

	UpdateSubresources<1>(
		InCommandList,
		OutTexture.Get(),
		OutUploadBuffer.Get(),
		0,
		0,
		1,
		&SubresourceData);

	CD3DX12_RESOURCE_BARRIER ToShaderResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		OutTexture.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	InCommandList->ResourceBarrier(1, &ToShaderResourceBarrier);

	return S_OK;
}

static bool TryLoadTextureByKey(
	FRenderingTextureResourcesUpdate* InResourcesUpdate,
	const std::wstring& InTextureKey)
{
	if (InResourcesUpdate == nullptr || InTextureKey.empty())
	{
		return false;
	}

	def_c_paths Paths;
	init_def_c_paths(&Paths);

	std::string AssetPath =
		FEnginePathHelper::RelativeToAbsolutePath(
			FEnginePathHelper::GetEngineAssetPath());

	find_files(AssetPath.c_str(), &Paths, true);

	for (int i = 0; i < Paths.index; i++)
	{
		if (find_string(Paths.paths[i], ".png", 0) == -1 &&
			find_string(Paths.paths[i], ".PNG", 0) == -1 &&
			find_string(Paths.paths[i], ".jpg", 0) == -1 &&
			find_string(Paths.paths[i], ".JPG", 0) == -1 &&
			find_string(Paths.paths[i], ".jpeg", 0) == -1 &&
			find_string(Paths.paths[i], ".JPEG", 0) == -1)
		{
			continue;
		}

		char Filename[1024] = { 0 };
		get_path_clean_filename(Filename, Paths.paths[i]);

		std::string FilenameString = Filename;
		size_t DotPos = FilenameString.find_last_of('.');
		if (DotPos != std::string::npos)
		{
			FilenameString = FilenameString.substr(0, DotPos);
		}

		wchar_t FilenameW[1024] = { 0 };
		char_to_wchar_t(FilenameW, 1024, FilenameString.c_str());

		if (InTextureKey == FilenameW)
		{
			normalization_path(Paths.paths[i]);

			wchar_t TexturePath[1024] = { 0 };
			char_to_wchar_t(TexturePath, 1024, Paths.paths[i]);
			InResourcesUpdate->LoadTextureResources(TexturePath);
			return true;
		}
	}

	return false;
}

FRenderingTextureResourcesUpdate::FRenderingTextureResourcesUpdate()
{
	memset(&ShaderResourceViewDesc, 0, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));

	BuildParam();
}

void FRenderingTextureResourcesUpdate::LoadTextureResources(const wstring& InFilename)
{
	if (!IsSupportedTextureFile(InFilename.c_str()))
	{
		return;
	}

	unique_ptr<FRenderingTexture> MyTexture = std::make_unique<FRenderingTexture>();
	MyTexture->Filename = InFilename;
	BuildTextureAssetPaths(*MyTexture);

	HRESULT LoadResult = S_OK;
	if (IsDDSFile(InFilename.c_str()))
	{
		// 读取DDS数据
		// DDSデータを読み込む
		LoadResult = CreateDDSTextureFromFile12(
			GetD3dDevice().Get(),
			GetGraphicsCommandList().Get(),
			MyTexture->Filename.c_str(),
			MyTexture->Data,
			MyTexture->UploadBuffer);
	}
	else
	{
		LoadResult = CreateWICTextureFromFile12(
			GetD3dDevice().Get(),
			GetGraphicsCommandList().Get(),
			MyTexture->Filename.c_str(),
			MyTexture->Data,
			MyTexture->UploadBuffer);
	}

	if (FAILED(LoadResult))
	{
		return;
	}

	MyTexture->RenderingTextureID = 0;

	TexturesMapping[MyTexture->Name] = std::move(MyTexture);
}

void FRenderingTextureResourcesUpdate::BuildTextureConstantBuffer(ID3D12DescriptorHeap* InHeap, int Offset)
{
	UINT DescriptorOffset = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE Handle(InHeap->GetCPUDescriptorHandleForHeapStart());
	Handle.Offset(Offset, DescriptorOffset);

	for (auto& Tmp : TexturesMapping)
	{
		Tmp.second->RenderingTextureID = Offset;

		// 根据类型初始化对应贴图
		// タイプに応じて対応するテクスチャを初期化する
		ResetTextureByType(&Tmp.second);

		GetD3dDevice()->CreateShaderResourceView(
			Tmp.second->Data.Get(),
			&ShaderResourceViewDesc, Handle);

		Handle.Offset(1, DescriptorOffset);
		Offset++;
	}
}

void FRenderingTextureResourcesUpdate::BuildParam()
{
	ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	ShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
}


void FRenderingTextureResourcesUpdate::ResetTextureByType(std::unique_ptr<FRenderingTexture>* InTexture)
{
	// 确定当前格式
	// 現在のフォーマットを決定する
	ShaderResourceViewDesc.Format = (*InTexture)->Data->GetDesc().Format;

	switch (ShaderResourceViewDesc.ViewDimension)
	{
	case D3D12_SRV_DIMENSION_TEXTURE2D:
	{
		ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		ShaderResourceViewDesc.Texture2D.MipLevels = (*InTexture)->Data->GetDesc().MipLevels;
		ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.f;
		ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		break;
	}
	case D3D12_SRV_DIMENSION_TEXTURECUBE:
	{
		ShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		ShaderResourceViewDesc.TextureCube.MipLevels = (*InTexture)->Data->GetDesc().MipLevels;
		ShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.f;
		break;
	}
	}
}



void FRenderingTextureResourcesUpdate::SetViewDimension(D3D12_SRV_DIMENSION InNewDimension)
{
	ShaderResourceViewDesc.ViewDimension = InNewDimension;
}

std::unique_ptr<FRenderingTexture>* FRenderingTextureResourcesUpdate::FindRenderingTexture(const std::string& InKey)
{
	if (!InKey.empty())
	{
		// 宽字符转化
		// ワイド文字変換
		const char* InString = InKey.c_str();
		wchar_t TexturePath[1024] = { 0 };

		char_to_wchar_t(TexturePath, 1024, InString);

		if (TexturesMapping.find(TexturePath) != TexturesMapping.end())//key
		{
			return &TexturesMapping[TexturePath];
		}
		else if (TryLoadTextureByKey(this, TexturePath))
		{
			auto LoadedTexture = TexturesMapping.find(TexturePath);
			if (LoadedTexture != TexturesMapping.end())
			{
				return &LoadedTexture->second;
			}
		}
		else
		{
			for (auto& Tmp : TexturesMapping)
			{
				if (Tmp.second->Filename == TexturePath)
				{
					return &Tmp.second;
				}

				if (Tmp.second->AssetFilename == TexturePath)
				{
					return &Tmp.second;
				}

				if (Tmp.second->SimpleAssetFilename == TexturePath)
				{
					return &Tmp.second;
				}
			}
		}

	}

	return NULL;
}
