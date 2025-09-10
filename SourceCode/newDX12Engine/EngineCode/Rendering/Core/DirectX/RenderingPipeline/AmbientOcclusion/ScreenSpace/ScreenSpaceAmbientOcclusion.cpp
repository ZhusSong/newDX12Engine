#include "ScreenSpaceAmbientOcclusion.h"

void FScreenSpaceAmbientOcclusion::Init(
	FGeometryMap* InGeometryMap, 
	FDirectXPipelineState* InDirectXPipelineState,
	FRenderLayerManager* InRenderLayer)
{
	NormalBuffer.Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);
}

void FScreenSpaceAmbientOcclusion::Init(int InWidth, int InHeight)
{
	NormalBuffer.Init(InWidth, InHeight);
}

void FScreenSpaceAmbientOcclusion::Draw(float DeltaTime)
{
	NormalBuffer.Draw(DeltaTime);
}

void FScreenSpaceAmbientOcclusion::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	NormalBuffer.UpdateCalculations(DeltaTime, ViewportInfo);
}


void FScreenSpaceAmbientOcclusion::BuildDescriptors()
{
	NormalBuffer.BuildDescriptors();
	NormalBuffer.BuildRenderTargetRTV();
	NormalBuffer.BuildSRVDescriptors();
	NormalBuffer.BuildRTVDescriptors();
}
