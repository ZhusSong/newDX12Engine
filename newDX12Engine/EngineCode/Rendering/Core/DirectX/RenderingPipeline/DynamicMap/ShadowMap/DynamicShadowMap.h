//25.6.25 李
#pragma once
#include "../Core/DynamicMap.h"

class GClientViewport;
class FRenderLayerManager;

struct FGeometryMap;
struct FDirectXPipelineState;
struct FViewportInfo;

class FDynamicShadowMap :public FDynamicMap
{
	typedef FDynamicMap Super;
public:

	FDynamicShadowMap();

public:
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);
	virtual void Init(int InWidth, int InHeight);
	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
public:
	void GetViewportMatrix(XMFLOAT4X4& OutViewMatrix, XMFLOAT4X4& OutProjectMatrix);
public:
	void DrawShadowMapTexture(float DeltaTime);
public:
	void SetViewportPosition(const fvector_3d& InPosition);
	void SetViewportRotation(const fvector_3d& InRotation);
	void BuildViewMatrix(float DeltaTime);

	// 构建平行光阴影矩阵
	void BuildParallelLightMatrix(
		const fvector_3d& InDirection,
		const fvector_3d& InTargetPosition,
		float InRadius = 100.f);

	// 构建聚光灯阴影矩阵
	void BuildSpotLightMatrix(
		const fvector_3d& InDirection,
		const fvector_3d& InPosition,
		float InRadius = 100.f);
public:
	virtual void BuildViewport(const fvector_3d& InCenterPoint);

public:
	virtual void BuildRenderTargetDescriptor();
	virtual void BuildDepthStencilDescriptor();

protected:
	virtual void BuildRenderTargetSRV();

	// 阴影视口
	GClientViewport* ShadowViewport;
};