#include "PlanarMapRenderTarget.h"
#include "../../../../../Debug/EngineDebug.h"

FPlanarMapRenderTarget::FPlanarMapRenderTarget()
	:Super()
{
}

void FPlanarMapRenderTarget::Init(UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat)
{
	Super::Init(InWidth, InHeight, InFormat);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE& FPlanarMapRenderTarget::GetCPURenderTargetView(int Index)
{
	return RenderTargetViewCPU;
}

void FPlanarMapRenderTarget::BuildRenderTargetMap()
{
	D3D12_RESOURCE_DESC ResourceDesc = {};
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

	const float ClearColor[] = { 0.f, 0.f, 0.f, 1.f };
	CD3DX12_CLEAR_VALUE ClearValue(Format, ClearColor);
	CD3DX12_HEAP_PROPERTIES BufferProperties(D3D12_HEAP_TYPE_DEFAULT);

	ANALYSIS_HRESULT(GetD3dDevice()->CreateCommittedResource(
		&BufferProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&ClearValue,
		IID_PPV_ARGS(RenderTargetMap.ReleaseAndGetAddressOf())));

}

void FPlanarMapRenderTarget::BuildRTVDescriptors()
{
	if (RenderTargetViewCPU.ptr == 0)
	{
		return;
	}

	D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
	RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Format = Format;
	RTVDesc.Texture2D.MipSlice = 0;
	RTVDesc.Texture2D.PlaneSlice = 0;

	GetD3dDevice()->CreateRenderTargetView(
		RenderTargetMap.Get(),
		&RTVDesc,
		RenderTargetViewCPU);
}

void FPlanarMapRenderTarget::BuildSRVDescriptors()
{
	if (CPUShaderResourceView.ptr == 0)
	{
		return;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.Format = Format;
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	GetD3dDevice()->CreateShaderResourceView(
		RenderTargetMap.Get(),
		&SRVDesc,
		CPUShaderResourceView);
}
