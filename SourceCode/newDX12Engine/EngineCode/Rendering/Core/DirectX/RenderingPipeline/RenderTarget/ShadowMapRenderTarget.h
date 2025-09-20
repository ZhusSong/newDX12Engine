#pragma once
#include "Core/RenderTarget.h"

//提供渲染内容的接口
class FShadowMapRenderTarget :public FRenderTarget
{
	typedef FRenderTarget Super;

	friend class FDynamicShadowMap;
public:
	FShadowMapRenderTarget();

	virtual void Init(UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat);

protected:
	virtual void BuildRenderTargetMap();
	virtual void BuildSRVDescriptors();
	virtual void BuildDSVDescriptors();

protected:
	void BuildShadowConstantBuffer();

protected:
	CD3DX12_CPU_DESCRIPTOR_HANDLE DSVDes;//深度模板描述
};