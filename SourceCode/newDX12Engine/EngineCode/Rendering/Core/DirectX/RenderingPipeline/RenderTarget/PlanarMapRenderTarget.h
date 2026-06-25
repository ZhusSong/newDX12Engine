#pragma once
#include "Core/RenderTarget.h"

class FPlanarReflectionMap;

// 提供平面反射渲染目标的接口
// 平面反射レンダーターゲットを提供するインターフェース
class FPlanarMapRenderTarget : public FRenderTarget
{
	typedef FRenderTarget Super;
	friend class FPlanarReflectionMap;
public:
	FPlanarMapRenderTarget();

	void Init(UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat);
	CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCPURenderTargetView(int Index);

private:
	virtual void BuildRenderTargetMap();
	virtual void BuildSRVDescriptors();
	virtual void BuildRTVDescriptors();
};
