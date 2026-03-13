#pragma once
#include "../../EngineMinimal.h"
#include "../../Interface/DirectXDeviceInterface.h"

// 纹理资源缓冲区结构体
// テクスチャリソースバッファ構造体
struct FRenderingTexture
{
	FRenderingTexture()
		:RenderingTextureID(0)
	{}

	ComPtr<ID3D12Resource> UploadBuffer;
	ComPtr<ID3D12Resource> Data;// 资源        // リソース
	 
	wstring Name;// 贴图名                    // テクスチャ名
	wstring Filename;// 路径                  // パス
	wstring AssetFilename;// 资源文件夹路径 Texture'Asset/Texture/Hello.Hello'   // リソースフォルダのパス Texture'Asset/Texture/Hello.Hello'
	wstring SimpleAssetFilename;//  简易路径 Asset/Texture/Hello.Hello           // 簡易パス Asset/Texture/Hello.Hello

	UINT RenderingTextureID;  // 贴图ID       // テクスチャID
};

class FRenderingTextureResourcesUpdate
	:public enable_shared_from_this<FRenderingTextureResourcesUpdate>
	, public IDirectXDeviceInterface
{
public:
	FRenderingTextureResourcesUpdate();


	void LoadTextureResources(const wstring& InFilename);
	void BuildTextureConstantBuffer(ID3D12DescriptorHeap* InHeap, int Offset = 0);
public:
	void BuildParam();
	void ResetTextureByType(std::unique_ptr<FRenderingTexture>* InTexture);
	void SetViewDimension(D3D12_SRV_DIMENSION InNewDimension);

	FORCEINLINE UINT Size() const { return (UINT)TexturesMapping.size(); }

	std::unique_ptr<FRenderingTexture>* FindRenderingTexture(const std::string& InKey);

protected:
	std::map<std::wstring, std::unique_ptr<FRenderingTexture>> TexturesMapping;

	D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
};