#include "DynamicMap.h"
#include "../../Geometry/GeometryMap.h"
#include "../../PipelineState/DirectXPipelineState.h"
#include "../../RenderLayer/RenderLayerManager.h"
#include "../../../../../../Config/EngineRenderConfig.h"

FDynamicMap::FDynamicMap()
	: GeometryMap(NULL)
	, DirectXPipelineState(NULL)
	, RenderLayer(NULL)
	, Width(256)
	, Height(256)
{

}

void FDynamicMap::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{

}

void FDynamicMap::Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer)
{
	GeometryMap = InGeometryMap;
	DirectXPipelineState = InDirectXPipelineState;
	RenderLayer = InRenderLayer;
}

void FDynamicMap::PreDraw(float DeltaTime)
{

}

void FDynamicMap::Draw(float DeltaTime)
{

}
