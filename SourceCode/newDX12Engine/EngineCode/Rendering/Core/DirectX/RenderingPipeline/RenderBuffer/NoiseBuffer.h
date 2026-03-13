#pragma once
#include "Core/RenderBuffer.h"

// 噪声缓冲区
// ノイズバッファ
class FNoiseBuffer :public FRenderBuffer
{
	typedef FRenderBuffer Super;

public:
	FNoiseBuffer();

	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void Init(int InWidth, int InHeight) override;

	void Draw(float DeltaTime);

	virtual void BuildDescriptors();
	virtual void BuildRenderTargetRTV();
	virtual void BuildSRVDescriptors();
	virtual void BuildRTVDescriptors();

	// 构建上传堆
	// アップロードヒープを構築する
	virtual void BuildUploadBuffer(const D3D12_RESOURCE_DESC& InResourceDesc, ComPtr<ID3D12Resource>& InResource);
	
	D3D12_SUBRESOURCE_DATA GetSubResourceData(XMCOLOR* InBuffer, int InWidth, int InHeight);
	void UpdateToDefaultResource(UINT NumSubResources, const D3D12_SUBRESOURCE_DATA& InSubResource, ComPtr<ID3D12Resource>& InResource);

protected:
	// 构建渲染目标参数
	// レンダーターゲットパラメータを構築する
	virtual void BuildRenderTargetBuffer(ComPtr<ID3D12Resource>& OutResource);

protected:
	// 噪波上传堆
	// ノイズアップロードヒープ
	ComPtr<ID3D12Resource> NoiseMapUploadBuffer;
};