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
	, SRVOffset(0)
	, RTVOffset(0)
{

}

void FDynamicMap::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{

}

void FDynamicMap::BuildSRVOffset()
{
	UINT CBVDescriptorSize = GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();

	auto CPUSRVDesHeapStart = GeometryMap->GetHeap()->GetCPUDescriptorHandleForHeapStart();
	auto GPUSRVDesHeapStart = GeometryMap->GetHeap()->GetGPUDescriptorHandleForHeapStart();

	int Offset = GetSRVOffset();

	RenderTarget->GetCPUSRVOffset() =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(CPUSRVDesHeapStart,
			Offset,
			CBVDescriptorSize);

	RenderTarget->GetGPUSRVOffset() =
		CD3DX12_GPU_DESCRIPTOR_HANDLE(GPUSRVDesHeapStart,
			Offset,
			CBVDescriptorSize);
}

void FDynamicMap::BuildRTVOffset()
{
	RenderTarget.get()->GetCPURenderTargetView() =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			GetRTVHeap()->GetCPUDescriptorHandleForHeapStart(),
			GetRTVOffset(),
			GetDescriptorHandleIncrementSizeByRTV());
}
void FDynamicMap::Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer)
{
	GeometryMap = InGeometryMap;
	DirectXPipelineState = InDirectXPipelineState;
	RenderLayer = InRenderLayer;
}

void FDynamicMap::Init(int InWidth, int InHeight)
{
	Width = InWidth;
	Height = InHeight;
}

void FDynamicMap::PreDraw(float DeltaTime)
{

}

void FDynamicMap::Draw(float DeltaTime)
{

}
