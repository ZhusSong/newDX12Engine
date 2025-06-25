//25.6.18 李
#pragma once
#include "../Core/DynamicMap.h"
#include "../../RenderTarget/CubeMapRenderTarget.h"

class GClientViewport;
class FRenderLayerManager;

struct FViewportInfo;
struct FGeometryMap;
struct FDirectXPipelineState;

class FDynamicCubeMap :public FDynamicMap
{
	typedef FDynamicMap Super;

	struct FTmpViewportCapture
	{
		FTmpViewportCapture() {}
		FTmpViewportCapture(const fvector_3d& InCenterPoint);

		fvector_3d TargetPoint[6];
		fvector_3d UP[6];

		void BuildViewportCapture(const fvector_3d& InCenterPoint);
	};

public:
	FDynamicCubeMap();

	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);
	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);

public:
	void SetViewportPosition(const fvector_3d& InPosition);
	bool IsExitDynamicReflectionMesh();
public:
	virtual void BuildViewport(const fvector_3d& InCenterPoint);
	virtual void BuildDepthStencil();

	virtual void BuildDepthStencilDescriptor();
	virtual void BuildRenderTargetDescriptor();

protected:
	virtual void BuildRenderTargetRTV();
	virtual void BuildRenderTargetSRV();

protected:

	CD3DX12_CPU_DESCRIPTOR_HANDLE DSVDes;//深度模板描述

	std::vector<GClientViewport*> CubeMapViewport;//6个视口

	ComPtr<ID3D12Resource> DepthStencilBuffer;
};