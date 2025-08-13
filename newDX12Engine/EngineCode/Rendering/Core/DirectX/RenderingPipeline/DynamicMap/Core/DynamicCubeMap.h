//25.7.2 李
#pragma once
#include "DynamicMap.h"
#include "../../RenderTarget/CubeMapRenderTarget.h"

class FRenderLayerManager;
class GClientViewport;

struct FGeometryMap;
struct FDirectXPipelineState;
struct FViewportInfo;

class FDynamicCubeMap :public FDynamicMap
{
	typedef FDynamicMap Super;
protected:
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

	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);

	void SetViewportPosition(const fvector_3d& InPosition);
	virtual void BuildViewport(const fvector_3d& InCenterPoint);

	virtual void BuildDepthStencil();
	virtual void BuildRenderTargetDescriptor();

protected:
		virtual void BuildRenderTargetRTV() {}
		virtual void BuildRenderTargetSRV() {}

protected:

	CD3DX12_CPU_DESCRIPTOR_HANDLE DSVDes;//深度模板描述

	std::vector<GClientViewport*> CubeMapViewport;//6个视口

	ComPtr<ID3D12Resource> DepthStencilBuffer;
};
