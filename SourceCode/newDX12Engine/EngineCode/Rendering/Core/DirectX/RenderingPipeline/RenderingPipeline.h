#pragma once
#include "../../../../Interface/DirectXDeviceInterface.h"
#include "Geometry/GeometryMap.h"
#include "../../../../Shader/Core/Shader.h"
#include "PipelineState/DirectXPipelineState.h"
#include "RootSignature/DefaultDirectXRootSignature.h"
#include "../../../../Core/Viewport/ViewportInfo.h"

#include "RenderLayer/RenderLayerManager.h"
#include "DynamicMap/CubeMap/DynamicReflectionCubeMap.h"

#include "UI/IMGUIPipeline.h"

#include "AmbientOcclusion/ScreenSpace/ScreenSpaceAmbientOcclusion.h"

class CMeshComponent;

// 渲染流水线基类
// レンダリングパイプラインの基底クラス
class FRenderingPipeline :public IDirectXDeviceInterface
{
public:
	FRenderingPipeline();

	// 构建Mesh
	// メッシュを構築する
	void BuildMesh(const size_t InMeshHash, CMeshComponent* InMesh, const FMeshRenderingData& MeshData);
	void DuplicateMesh(CMeshComponent* InMesh, std::shared_ptr<FRenderingData>& MeshData);
	bool FindMeshRenderingDataByHash(const size_t& InHash, std::shared_ptr<FRenderingData>& MeshData, int InRenderLayerIndex = -1);

	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	virtual void OnResetSize(int InWidth, int InHeight);
	// 构建渲染流水线
	// レンダリングパイプラインを構築する
	void BuildPipeline();

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);

public:
	FRenderLayerManager* GetRenderLayer() const { return const_cast<FRenderLayerManager*>(&RenderLayer); }

protected:

	// 渲染层级
	// レンダリングレイヤー
	FRenderLayerManager RenderLayer;
	
	// 渲染流水线状态
	// レンダリングパイプライン状態
	FDirectXPipelineState DirectXPipelineState;
	// 根签名
	// ルートシグネチャ
	FDefaultDirectXRootSignature RootSignature;
	// 几何图形
	// ジオメトリマップ
	FGeometryMap GeometryMap;
	// 动态cubemap
	// 動的Cubemap
	FDynamicReflectionCubeMap DynamicCubeMap;

	// UI
	FIMGUIPipeline UIPipeline;


	// SSAO
	FScreenSpaceAmbientOcclusion SSAO;

};