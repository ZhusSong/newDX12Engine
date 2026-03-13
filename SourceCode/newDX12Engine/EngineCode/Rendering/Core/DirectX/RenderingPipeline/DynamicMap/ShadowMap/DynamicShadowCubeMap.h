#pragma once
#include "../Core/DynamicCubeMap.h"


class GClientViewport;
class FRenderLayerManager;

struct FGeometryMap;
struct FDirectXPipelineState;
struct FViewportInfo;

class FDynamicShadowCubeMap :public FDynamicCubeMap
{
	typedef FDynamicCubeMap Super;

public:
	FDynamicShadowCubeMap();
	virtual void OnResetSize(int InWidth, int InHeight);

	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);
	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
public:

	virtual void BuildDepthStencilDescriptor();

protected:
	// 构建RTV
	// RTVを構築
	virtual void BuildRenderTargetRTV();

	// 构建SRV
	// SRVを構築
	virtual void BuildRenderTargetSRV();
};