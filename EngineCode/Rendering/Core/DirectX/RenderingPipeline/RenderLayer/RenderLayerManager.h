
//25.6.10 李
#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"
#include "Core/RenderLayer.h"

class FRenderLayerManager
{
	friend class FRenderLayer;
	friend struct FGeometry;
	friend struct FGeometryMap;
public:
	FRenderLayerManager();
	~FRenderLayerManager();

	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState);
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);

	virtual void BuildShader();

	//排序
	virtual void Sort();

	static std::shared_ptr<FRenderLayer> FindByRenderLayer(int InRenderLayer);
protected:
	static std::vector<std::shared_ptr<FRenderLayer>> RenderLayers;
};

//注册渲染层级
template<class T>
std::shared_ptr<T> CreateRenderLayer()
{
	std::shared_ptr<T> RenderLayer = std::make_shared<T>();
	RenderLayer->RegisterRenderLayer();

	return RenderLayer;
}