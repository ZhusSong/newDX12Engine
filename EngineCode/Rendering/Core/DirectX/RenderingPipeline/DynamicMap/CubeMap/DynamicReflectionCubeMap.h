//25.6.18 李
#pragma once
#include "../Core/DynamicCubeMap.h"
#include "../../RenderTarget/CubeMapRenderTarget.h"

class GClientViewport;
class FRenderLayerManager;

struct FViewportInfo;
struct FGeometryMap;
struct FDirectXPipelineState;

// 动态反射CubeMap
class FDynamicReflectionCubeMap :public FDynamicCubeMap
{
	typedef FDynamicCubeMap Super;

public:
	FDynamicReflectionCubeMap();

	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);
	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);

public:
	bool IsExitDynamicReflectionMesh();
public:
	virtual void BuildDepthStencilDescriptor();
protected:
	virtual void BuildRenderTargetRTV();
	virtual void BuildRenderTargetSRV();
};