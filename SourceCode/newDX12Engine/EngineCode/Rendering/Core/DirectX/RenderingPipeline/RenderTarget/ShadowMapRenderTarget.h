#pragma once
#include "Core/RenderTarget.h"

// 提供渲染内容的接口
// 描画内容を提供するインターフェース
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
	// 深度模板描述
	// デプスステンシル記述
	CD3DX12_CPU_DESCRIPTOR_HANDLE DSVDes;
};