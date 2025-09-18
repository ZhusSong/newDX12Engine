#include "AmbientBuffer.h"
#include "../RenderTarget/BufferRenderTarget.h"
#include "../Geometry/GeometryMap.h"
#include "../RenderLayer/RenderLayerManager.h"
#include "../../../../../Component/Mesh/Core/MeshComponentType.h"
#include "../../../../../Config/EngineRenderConfig.h"

FAmbientBuffer::FAmbientBuffer()
{
	Format = DXGI_FORMAT_R16_UNORM;
}

void FAmbientBuffer::Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer)
{
	Super::Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);
}

void FAmbientBuffer::Init(int InWidth, int InHeight)
{
	Super::Init(InWidth, InHeight);
}

void FAmbientBuffer::Draw(float DeltaTime)
{

}

// 创建描述符
void FAmbientBuffer::BuildDescriptors()
{
	UINT CBVDescriptorSize = GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();

	auto CPUSRVDesHeapStart = GeometryMap->GetHeap()->GetCPUDescriptorHandleForHeapStart();
	auto GPUSRVDesHeapStart = GeometryMap->GetHeap()->GetGPUDescriptorHandleForHeapStart();

	int Offset =
		GeometryMap->GetDrawTexture2DResourcesNumber() + //Texture2D
		GeometryMap->GetDrawCubeMapResourcesNumber() + //静态Cube贴图 背景 天空球
		1 + //动态Cube贴图 反射
		1 + //Shadow 直射灯 聚光灯 Shadow
		1 + //ShadowCubeMap 点光源的 Shadow
		1 + //UI
		1 +//法线
		1 +  //深度
		1;  //Noise 噪声图

	RenderTarget->GetCPUSRVOffset() =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(CPUSRVDesHeapStart,
			Offset,
			CBVDescriptorSize);

	RenderTarget->GetGPUSRVOffset() =
		CD3DX12_GPU_DESCRIPTOR_HANDLE(GPUSRVDesHeapStart,
			Offset,
			CBVDescriptorSize);
}

// 构建RTV
void FAmbientBuffer::BuildRenderTargetRTV()
{
	UINT RTVDescriptorSize = GetDescriptorHandleIncrementSizeByRTV();

	auto RTVDesHeapStart = GetRTVHeap()->GetCPUDescriptorHandleForHeapStart();

	int Offset =
		FEngineRenderConfig::GetRenderConfig()->SwapChainCount +//交换链
		6 +//反射的CubeMap RTV
		6 +//ShadowCubeMap RTV Point Light
		1; //Normal

	if (FBufferRenderTarget* InRenderTarget = dynamic_cast<FBufferRenderTarget*>(RenderTarget.get()))
	{
		InRenderTarget->GetCPURenderTargetView() =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVDesHeapStart, Offset, RTVDescriptorSize);
	}
}

void FAmbientBuffer::BuildSRVDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.Format = Format;
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	GetD3dDevice()->CreateShaderResourceView(
		RenderTarget->GetRenderTarget(),
		&SRVDesc,
		RenderTarget->GetCPUSRVOffset());
}


void FAmbientBuffer::BuildRTVDescriptors()
{
	if (FBufferRenderTarget* InRenderTarget = dynamic_cast<FBufferRenderTarget*>(RenderTarget.get()))
	{
		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = { };
		RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Format = Format;
		RTVDesc.Texture2D.MipSlice = 0;
		RTVDesc.Texture2D.PlaneSlice = 0;

		GetD3dDevice()->CreateRenderTargetView(
			InRenderTarget->GetRenderTarget(),
			&RTVDesc,
			InRenderTarget->GetCPURenderTargetView());
	}
}


void FAmbientBuffer::BuildRenderTargetBuffer(ComPtr<ID3D12Resource>& OutResource)
{
	D3D12_RESOURCE_DESC ResourceDesc;
	memset(&ResourceDesc, 0, sizeof(ResourceDesc));

	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Width = Width;
	ResourceDesc.Height = Height;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = Format;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	CD3DX12_HEAP_PROPERTIES BufferProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	const float AmbientColor[] = { 1.f,1.f,1.f,1.f };
	CD3DX12_CLEAR_VALUE ClearValue(Format, AmbientColor);

	ANALYSIS_HRESULT(GetD3dDevice()->CreateCommittedResource(
		&BufferProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&ClearValue,
		IID_PPV_ARGS(OutResource.GetAddressOf())));
}
