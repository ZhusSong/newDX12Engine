#pragma once
#include "Core/RenderTarget.h"

DEFINITION_SIMPLE_SINGLE_DELEGATE(FRenderTargetDelegate, void, ComPtr<ID3D12Resource>&)

class FBufferRenderTarget :public FRenderTarget
{
	typedef FRenderTarget Super;

public:
	FRenderTargetDelegate RenderTargetDelegate;

	virtual void Init(UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat);

public:
	FBufferRenderTarget();

protected:
	virtual void BuildRenderTargetMap();
	virtual void BuildSRVDescriptors();
	virtual void BuildRTVDescriptors();

};