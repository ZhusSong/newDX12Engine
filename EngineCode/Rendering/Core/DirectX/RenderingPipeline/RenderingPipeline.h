#pragma once
#include "../../../../Interface/DirectXDeviceInterface.h"
#include "Geometry/GeometryMap.h"
#include "../../../../Shader/Core/Shader.h"
#include "PipelineState/DirectXPipelineState.h"
#include "RootSignature/DirectXRootSignature.h"
#include "../../../../Core/Viewport/ViewportInfo.h"

#include "RenderLayer/RenderLayerManager.h"
#include "DynamicMap/CubeMap/DynamicReflectionCubeMap.h"

#include "UI/IMGUIPipeline.h"

class CMeshComponent;

// 渲染流水线基类
class FRenderingPipeline :public IDirectXDeviceInterface
{
public:
	FRenderingPipeline();

	// 构建Mesh
	void BuildMesh(const size_t InMeshHash, CMeshComponent* InMesh, const FMeshRenderingData& MeshData);
	void DuplicateMesh(CMeshComponent* InMesh, std::shared_ptr<FRenderingData>& MeshData);
	bool FindMeshRenderingDataByHash(const size_t& InHash, std::shared_ptr<FRenderingData>& MeshData, int InRenderLayerIndex = -1);

	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	// 构建渲染流水线
	void BuildPipeline();

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);

public:
	FRenderLayerManager* GetRenderLayer() const { return const_cast<FRenderLayerManager*>(&RenderLayer); }

protected:

	// 渲染层级
	FRenderLayerManager RenderLayer;
	
	// 渲染流水线状态
	FDirectXPipelineState DirectXPipelineState;
	// 根签名
	FDirectXRootSignature RootSignature;
	// 几何图形
	FGeometryMap GeometryMap;
	// 动态cubemap
	FDynamicReflectionCubeMap DynamicCubeMap;

	// UI
	FIMGUIPipeline UIPipeline;

};