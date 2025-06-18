#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"

// 提供渲染目标的接口
class FCubeMapRenderTarget
	:public IDirectXDeviceInterface
	, std::enable_shared_from_this<FCubeMapRenderTarget>
{
	friend class FDynamicCubeMap;
public:
	FCubeMapRenderTarget();

	void Init(UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat);

	void ResetViewport(UINT InWidth, UINT InHeight);
	void ResetScissorRect(UINT InWidth, UINT InHeight);

	void ResetRenderTarget(UINT InWidth, UINT InHeight);

public:
	void BuildRenderTargetMap();
	void BuildSRVDescriptors();
	void BuildRTVDescriptors();

public:
	FORCEINLINE ID3D12Resource* GetRenderTarget() const { return RenderTargetMap.Get(); }
	FORCEINLINE D3D12_VIEWPORT GetViewport() const { return Viewport; }
	FORCEINLINE D3D12_RECT GetScissorRect()const { return ScissorRect; }

private:
	UINT Width;
	UINT Height;
	DXGI_FORMAT Format;

	D3D12_VIEWPORT Viewport;
	D3D12_RECT ScissorRect;

	ComPtr<ID3D12Resource> RenderTargetMap;

	//SRV
	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUShaderResourceView;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUShaderResourceView;

	//RTV
	std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> CPURenderTargetView;
};