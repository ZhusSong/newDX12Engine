//25.6.21 李
#pragma once
#include "Core/RenderTarget.h"

// 提供渲染目标的接口
class FCubeMapRenderTarget :public FRenderTarget
{
	typedef FRenderTarget Super;
	friend class FDynamicCubeMap;
public:
	FCubeMapRenderTarget();

	void Init(UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat);

private:
	virtual void BuildRenderTargetMap();
	virtual void BuildSRVDescriptors();
	virtual void BuildRTVDescriptors();

private:
	//RTV
	std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> CPURenderTargetView;
};