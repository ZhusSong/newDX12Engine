//25.6.25 李
#pragma once
#include "../../../../../../Interface/DirectXDeviceInterface.h"
#include "../../RenderTarget/Core/RenderTarget.h"

struct FGeometryMap;
struct FDirectXPipelineState;
class FRenderLayerManager;
struct FViewportInfo;

class FDynamicMap :public IDirectXDeviceInterface
{
public:
	FDynamicMap();

public:
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);
	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
public:
	template<class T>
	void CreateRenderTarget();

protected:
	FGeometryMap* GeometryMap;//几何Map
	FDirectXPipelineState* DirectXPipelineState;//管线对象 用于绑定

	FRenderLayerManager* RenderLayer;//渲染层级

	UINT Width;
	UINT Height;

	std::unique_ptr<FRenderTarget> RenderTarget;//渲染目标
};

template<class T>
void FDynamicMap::CreateRenderTarget()
{
	if (!RenderTarget)
	{
		RenderTarget = std::make_unique<T>();
	}
}
