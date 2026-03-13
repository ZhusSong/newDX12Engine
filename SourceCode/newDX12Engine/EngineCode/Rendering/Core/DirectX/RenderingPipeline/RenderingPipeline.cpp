#include "RenderingPipeline.h"
#include "../../../../Component/Mesh/Core/MeshComponentType.h"
#include "../../../../Config/EngineRenderConfig.h"


FRenderingPipeline::FRenderingPipeline()
{

}

void FRenderingPipeline::BuildMesh(const size_t InMeshHash, CMeshComponent* InMesh, const FMeshRenderingData& MeshData)
{
	GeometryMap.BuildMesh(InMeshHash, InMesh, MeshData);
}

void FRenderingPipeline::DuplicateMesh(CMeshComponent* InMesh, std::shared_ptr<FRenderingData>& MeshData)
{
	GeometryMap.DuplicateMesh(InMesh, MeshData);
}

bool FRenderingPipeline::FindMeshRenderingDataByHash(const size_t& InHash, std::shared_ptr<FRenderingData>& MeshData, int InRenderLayerIndex)
{
	return GeometryMap.FindMeshRenderingDataByHash(InHash, MeshData, InRenderLayerIndex);
}


void FRenderingPipeline::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	SSAO.UpdateCalculations(DeltaTime, ViewportInfo);
	GeometryMap.DynamicShadowCubeMap.UpdateCalculations(DeltaTime, ViewportInfo);

	DynamicCubeMap.UpdateCalculations(DeltaTime, ViewportInfo);
	GeometryMap.UpdateCalculations(DeltaTime, ViewportInfo);
	RenderLayer.UpdateCalculations(DeltaTime, ViewportInfo);
}

void FRenderingPipeline::OnResetSize(int InWidth, int InHeight)
{
	//SSAO.OnResetSize(InWidth, InHeight);
	DynamicCubeMap.OnResetSize(InWidth, InHeight);
	GeometryMap.OnResetSize(InWidth, InHeight);
	RenderLayer.OnResetSize(InWidth, InHeight);
}

void FRenderingPipeline::BuildPipeline()
{

	// 初始化GPS描述
	// GPS記述を初期化する
	DirectXPipelineState.ResetGPSDesc();

	// 渲染层级的初始化
	// 描画レイヤーを初期化する
	RenderLayer.Init(&GeometryMap, &DirectXPipelineState);

	// 对渲染层级进行排序
	// 描画レイヤーをソートする
	RenderLayer.Sort();

	// 读取贴图纹理
	// テクスチャをロードする
	GeometryMap.LoadTexture();

	// 构建雾
	// フォグを構築する
	GeometryMap.BuildFog();
	
	// 构建动态的CubeMap
	// 動的キューブマップを構築する
	DynamicCubeMap.Init(
		&GeometryMap,
		&DirectXPipelineState,
		&RenderLayer);

	//// 构建SSAO
	// SSAOを構築する
	SSAO.Init(
		&GeometryMap,
		&DirectXPipelineState,
		&RenderLayer);

	SSAO.Init(GetViewportWidth(), GetViewportHeight());



	// 构建普通阴影map
	// 通常のシャドウマップを構築する
	GeometryMap.DynamicShadowMap.Init(
		&GeometryMap,
		&DirectXPipelineState,
		&RenderLayer);

	// 构建Cubemap阴影
	// キューブマップシャドウを構築する
	GeometryMap.DynamicShadowCubeMap.Init(
		&GeometryMap,
		&DirectXPipelineState,
		&RenderLayer);

	// 构建根签名
	// ルートシグネチャを構築する
	RootSignature.BuildRootSignature(GeometryMap.GetDrawTexture2DResourcesNumber());
	DirectXPipelineState.BindRootSignature(RootSignature.GetRootSignature());

	// 构建模型
	// モデルを構築する
	GeometryMap.Build();

	// 构建动态反射Mesh
	// 動的反射メッシュを構築する
	GeometryMap.BuildDynamicReflectionMesh();

	// 构建常量描述堆
	// 定数ディスクリプタヒープを構築する
	GeometryMap.BuildDescriptorHeap();

	// 构建SSAO描述堆
	// 定数ディスクリプタヒープを構築する
	SSAO.BuildDescriptors();

	// 初始化UI管线
	// UIパイプラインを初期化する
	UIPipeline.Init(
		GeometryMap.GetHeap(),
		GeometryMap.GetDrawTexture2DResourcesNumber() + //Texture2D
		GeometryMap.GetDrawCubeMapResourcesNumber() + //静态Cube贴图 // 静的Cubeマップ  
		1 + //动态Cube贴图 // 動的Cubeマップ  
		1 + //Shadow
		1);//ShadowCubeMap

	// 初始化CubeMap 摄像机
	// キューブマップ用カメラを初期化する
	DynamicCubeMap.BuildViewport(fvector_3d(0.f, 0.f, 0.f));

	// 构建深度模板描述
	// デプスステンシル記述を構築する
	DynamicCubeMap.BuildDepthStencilDescriptor();

	// 构建RTVDes
	// RTV記述を構築する
	DynamicCubeMap.BuildRenderTargetDescriptor();

	// 构建深度模板
	// デプスステンシルを構築する
	DynamicCubeMap.BuildDepthStencil();

	//构建阴影
	// シャドウを構築する
	GeometryMap.BuildShadow();

	// 构建常量缓冲区
	// 定数バッファを構築する
	GeometryMap.BuildMeshConstantBuffer();

	// 构建材质常量缓冲区
	// マテリアル定数バッファを構築する
	GeometryMap.BuildMaterialShaderResourceView();

	// 构建灯光常量缓冲区
	// ライト定数バッファを構築する
	GeometryMap.BuildLightConstantBuffer();

	// 构建视口常量缓冲区视图
	// ビューポート定数バッファビューを構築する
	GeometryMap.BuildViewportConstantBufferView();

	// 构建贴图
	// テクスチャを構築する
	GeometryMap.BuildTextureConstantBuffer();

	// 构建雾气常量缓冲区
	// テクスチャを構築する
	GeometryMap.BuildFogConstantBuffer();

	//构建SSAO
	// SSAOを構築する
	SSAO.Build();

	// 存储一个默认的GPS描述数据
	// デフォルトのGPS記述データを保存する
	DirectXPipelineState.SaveGPSDescAsDefault();

	// 通过层级来构建PSO
	// レイヤーを通じてPSOを構築する
	RenderLayer.BuildPSO();
}

void FRenderingPipeline::PreDraw(float DeltaTime)
{

	DirectXPipelineState.PreDraw(DeltaTime);

	GeometryMap.PreDraw(DeltaTime);
	RootSignature.PreDraw(DeltaTime);

	
	// 渲染灯光材质贴图等
	// ライトやマテリアルのテクスチャなどを描画する
	GeometryMap.Draw(DeltaTime);

	// 渲染SSAO
	// SSAOを描画する
	//SSAO.Draw(DeltaTime);
	RootSignature.PreDraw(DeltaTime);

	// 存储SSAO到指定的buffer
	// SSAOを指定のバッファに保存する
	SSAO.SaveToSSAOBuffer();

	// 主视口清除画布
	// メインビューのCanvasをクリアする
	ClearMainSwapChainCanvas();

	//重新绑定贴图
	// テクスチャを再バインドする
	GeometryMap.Draw2DTexture(DeltaTime);

	// 渲染shadowCubeMap
	// shadowCubeMapを描画する
	GeometryMap.DynamicShadowCubeMap.PreDraw(DeltaTime);

	// 渲染阴影
	// シャドウを描画する
	GeometryMap.DrawShadow(DeltaTime);

	// 动态反射
	// 動的反射
	if (DynamicCubeMap.IsExitDynamicReflectionMesh())
	{
		DynamicCubeMap.PreDraw(DeltaTime);
	}

	RenderLayer.PreDraw(DeltaTime);
}

void FRenderingPipeline::Draw(float DeltaTime)
{
	// 主视口
	// メインビュー
	GeometryMap.DrawViewport(DeltaTime);

	// 绘制抓取到的ShadowCubeMap贴图
	// 取得したShadowCubeMapのテクスチャを描画する
	GeometryMap.DrawCubeMapTexture(DeltaTime);

	// 各类层级
	// 各レイヤー
	RenderLayer.Draw(RENDERLAYER_BACKGROUND, DeltaTime);
	RenderLayer.Draw(RENDERLAYER_OPAQUE, DeltaTime);
	RenderLayer.Draw(RENDERLAYER_TRANSPARENT, DeltaTime);

	// 渲染选择箭头
	// 選択用のハンドルを描画する
	RenderLayer.Draw(RENDERLAYER_SELECT, DeltaTime);

	// 渲染旋转面片
	// 回転ハンドルを描画する
	RenderLayer.Draw(RENDERLAYER_OPERATION_HANDLE_ROT_PLANE, DeltaTime);

	// 渲染操作手柄
	// 操作ハンドルを描画する
	RenderLayer.Draw(RENDERLAYER_OPERATION_HANDLE, DeltaTime);

	// 最后渲染UI
	// UIを描画する
	UIPipeline.Draw(DeltaTime);

	DirectXPipelineState.Draw(DeltaTime);
}

void FRenderingPipeline::PostDraw(float DeltaTime)
{
	GeometryMap.PostDraw(DeltaTime);
	RenderLayer.PostDraw(DeltaTime);
	DirectXPipelineState.PostDraw(DeltaTime);
}
