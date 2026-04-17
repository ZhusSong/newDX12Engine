#include "ScreenSpaceAmbientOcclusion.h"
#include "SSAOType.h"
#include "../../../../../../Core/Viewport/ViewportInfo.h"
#include "../../RenderLayer/RenderLayerManager.h"
#include "../../../../../../Component/Mesh/Core/MeshComponentType.h"
#include "../../RenderTarget/BufferRenderTarget.h"
#include "../../Geometry/GeometryMap.h"
#include "../../../../../../Config/EngineRenderConfig.h"

FScreenSpaceAmbientOcclusion::FScreenSpaceAmbientOcclusion()
{
	RenderLayer = NULL;
	BlurRadius = 5.f;
}

void FScreenSpaceAmbientOcclusion::Init(
	FGeometryMap* InGeometryMap,
	FDirectXPipelineState* InDirectXPipelineState,
	FRenderLayerManager* InRenderLayer)
{
	NormalBuffer.Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);
	AmbientBuffer.Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);
	NoiseBuffer.Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);
	BilateralBlur.Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);

	RenderLayer = InRenderLayer;
	GeometryMap = InGeometryMap;
}

void FScreenSpaceAmbientOcclusion::Init(int InWidth, int InHeight)
{
	NormalBuffer.Init(InWidth, InHeight);
	AmbientBuffer.Init(InWidth / 2.f, InHeight / 2.f);
	BilateralBlur.Init(InWidth / 2.f, InHeight / 2.f);
	NoiseBuffer.Init(InWidth, InHeight);
}

void FScreenSpaceAmbientOcclusion::OnResetSize(int InWidth, int InHeight)
{
	Init(InWidth, InHeight);

	BuildDescriptors();
}

void FScreenSpaceAmbientOcclusion::Draw(float DeltaTime)
{
	NormalBuffer.Draw(DeltaTime);
	AmbientBuffer.Draw(DeltaTime);
	NoiseBuffer.Draw(DeltaTime);
	BilateralBlur.Draw(DeltaTime);

	DirectXRootSignature.PreDraw(DeltaTime);

	DrawResources(DeltaTime);

	DrawSSAO(DeltaTime);
}

void FScreenSpaceAmbientOcclusion::DrawSSAO(float DeltaTime)
{
	// 主SSAO渲染
	// メインSSAO描画
	if (FBufferRenderTarget* InRenderTarget = dynamic_cast<FBufferRenderTarget*>(AmbientBuffer.GetRenderTarget().get()))
	{
		auto RenderTargetViewport = InRenderTarget->GetViewport();
		auto RenderTargetScissorRect = InRenderTarget->GetScissorRect();

		GetGraphicsCommandList()->RSSetViewports(1, &RenderTargetViewport);
		GetGraphicsCommandList()->RSSetScissorRects(1, &RenderTargetScissorRect);

		// 转换资源状态
		// リソース状態を変換
		CD3DX12_RESOURCE_BARRIER ResourceBarrierPresent = CD3DX12_RESOURCE_BARRIER::Transition(
			InRenderTarget->GetRenderTarget(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
		GetGraphicsCommandList()->ResourceBarrier(1, &ResourceBarrierPresent);

		const float ClearColor[] = { 1.f,1.f,1.f,1.f };
		GetGraphicsCommandList()->ClearRenderTargetView(
			InRenderTarget->GetCPURenderTargetView(),
			ClearColor, 0, nullptr);

		GetGraphicsCommandList()->OMSetRenderTargets(1,
			&InRenderTarget->GetCPURenderTargetView(),
			true, nullptr);

		// 渲染SSAOPSO
		// SSAO PSOを描画
		RenderLayer->Draw(RENDERLAYER_SSAO, DeltaTime);

		CD3DX12_RESOURCE_BARRIER ResourceBarrierPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
			InRenderTarget->GetRenderTarget(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
		GetGraphicsCommandList()->ResourceBarrier(1, &ResourceBarrierPresentRenderTarget);
	}
}

void FScreenSpaceAmbientOcclusion::DrawBilateralBlur(float DeltaTime, UINT InDrawNum)
{
	for (size_t i = 0; i < InDrawNum; i++)
	{
		DrawBlurHorizontal(DeltaTime);
		DrawBlurVertical(DeltaTime);
	}
}

void FScreenSpaceAmbientOcclusion::DrawBlurHorizontal(float DeltaTime)
{
	DrawBlur(DeltaTime, true);
}

void FScreenSpaceAmbientOcclusion::DrawBlurVertical(float DeltaTime)
{
	DrawBlur(DeltaTime, false);
}

ID3D12Resource* FScreenSpaceAmbientOcclusion::GetDrawResources(bool bHorizontal)
{
	return bHorizontal ?
		BilateralBlur.GetRenderTarget()->GetRenderTarget() :
		AmbientBuffer.GetRenderTarget()->GetRenderTarget();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE* FScreenSpaceAmbientOcclusion::GetDrawSRVResources(bool bHorizontal)
{
	return bHorizontal ?
		&AmbientBuffer.GetRenderTarget()->GetGPUSRVOffset():
		&BilateralBlur.GetRenderTarget()->GetGPUSRVOffset();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE* FScreenSpaceAmbientOcclusion::GetDrawRTVResources(bool bHorizontal)
{
	return bHorizontal ?
		&BilateralBlur.GetRenderTarget()->GetCPURenderTargetView() :
		&AmbientBuffer.GetRenderTarget()->GetCPURenderTargetView();
}

void FScreenSpaceAmbientOcclusion::SetRoot32BitConstant(bool bHorizontal)
{
	GetGraphicsCommandList()->SetGraphicsRoot32BitConstant(1, (UINT)bHorizontal, 0);
}

void FScreenSpaceAmbientOcclusion::DrawResources(float DeltaTime)
{
	GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(
		0,
		SSAOViewConstantBufferViews.GetBuffer()->GetGPUVirtualAddress());

	// Nor + packed depth
	if (std::shared_ptr<FRenderTarget> NormalRenderTarget = NormalBuffer.GetRenderTarget())
	{
		GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
			3,
			NormalRenderTarget->GetGPUSRVOffset());
	}

	if (std::shared_ptr<FRenderTarget> DepthRenderTarget = NormalBuffer.GetDepthRenderTarget())
	{
		GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
			4,
			DepthRenderTarget->GetGPUSRVOffset());
	}

	if (std::shared_ptr<FRenderTarget> NoiseRenderTarget = NoiseBuffer.GetRenderTarget())
	{
		GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
			5,
			NoiseRenderTarget->GetGPUSRVOffset());
	}
}


void FScreenSpaceAmbientOcclusion::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	NormalBuffer.UpdateCalculations(DeltaTime, ViewportInfo);
	AmbientBuffer.UpdateCalculations(DeltaTime, ViewportInfo);
	NoiseBuffer.UpdateCalculations(DeltaTime, ViewportInfo);
	BilateralBlur.UpdateCalculations(DeltaTime, ViewportInfo);

	DrawViewConstantBufferViews(DeltaTime, ViewportInfo);
	DrawBlurConstantBufferViews(DeltaTime, ViewportInfo);
}

void FScreenSpaceAmbientOcclusion::BuildBlurWeights(float InSigma, bool bReBuildBlurWeights)
{
	if (bReBuildBlurWeights)
	{
		BlurWeights.clear();
	}

	if (BlurWeights.size() == 0)
	{
		BlurRadius = ceil(InSigma * 2.f);

		BlurWeights.resize(2.f * BlurRadius + 1);

		float Len = 2.f * pow(InSigma, 2);
		float Weights = 0.f;
		for (int i = -BlurRadius; i <= BlurRadius; ++i)
		{
			float Value = (float)i;
			BlurWeights[i + BlurRadius] = expf(-pow(Value, 2) / Len);
			Weights += BlurWeights[i + BlurRadius];
		}

		// 将权重值 映射到 0 - 1
		// 重み値を0～1にマッピング
		for (int i = 0; i < BlurWeights.size(); i++)
		{
			BlurWeights[i] /= Weights;
		}
	}
}

void FScreenSpaceAmbientOcclusion::DrawViewConstantBufferViews(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	FSSAOViewportTransformation SSAOViewportTransformation;

	// 逆矩阵
	// 逆行列
	XMMATRIX ProjectMatrixRIX = XMLoadFloat4x4(&ViewportInfo.ProjectMatrix);

	XMVECTOR ProjectDeterminant = XMMatrixDeterminant(ProjectMatrixRIX);
	XMMATRIX InversiveProjectionMatrixRIX = XMMatrixInverse(&ProjectDeterminant, ProjectMatrixRIX);
	XMStoreFloat4x4(&SSAOViewportTransformation.InversiveProjectionMatrix, XMMatrixTranspose(InversiveProjectionMatrixRIX));
	XMStoreFloat4x4(&SSAOViewportTransformation.ProjectionMatrix, XMMatrixTranspose(ProjectMatrixRIX));

	// 纹理空间
	// テクスチャ空間
	// [-1.1] =>[0,1] 
	XMMATRIX HalLambertMatrix(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX TexProjectionMatrixRIX = XMMatrixMultiply(ProjectMatrixRIX, HalLambertMatrix);
	XMStoreFloat4x4(&SSAOViewportTransformation.TexProjectionMatrix, XMMatrixTranspose(TexProjectionMatrixRIX));

	SSAOViewportTransformation.OcclusionRadius = 0.5f;
	SSAOViewportTransformation.OcclusionStart = 0.2f;
	SSAOViewportTransformation.OcclusionEnd = 2.0f;
	SSAOViewportTransformation.ObscurationThreshold = 0.05f;

	// 存储随机向量
	// ランダムベクトルを格納
	SampleVolumeBuffer.Update(SSAOViewportTransformation.SampleVolumeBuffer);

	// 上传
	// アップロード
	SSAOViewConstantBufferViews.Update(0, &SSAOViewportTransformation);
}

void FScreenSpaceAmbientOcclusion::DrawBlurConstantBufferViews(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	FSSAOBlurParam SSAOBlurParam = {};

	// 打包到 3 个 float4 中，末尾不足的权重补 0，避免越界读取。
	// 3 個の float4 にパックし、末尾が不足する重みは 0 で埋めて範囲外読み取りを防ぐ。
	for (int GroupIndex = 0; GroupIndex < 3; ++GroupIndex)
	{
		float PackedWeights[4] = { 0.f, 0.f, 0.f, 0.f };

		for (int WeightIndex = 0; WeightIndex < 4; ++WeightIndex)
		{
			int SourceIndex = GroupIndex * 4 + WeightIndex;
			if (SourceIndex < (int)BlurWeights.size())
			{
				PackedWeights[WeightIndex] = BlurWeights[SourceIndex];
			}
		}

		SSAOBlurParam.BlurWeights[GroupIndex] = XMFLOAT4(
			PackedWeights[0],
			PackedWeights[1],
			PackedWeights[2],
			PackedWeights[3]);
	}

	SSAOBlurParam.InversionSize = XMFLOAT2(1.f / BilateralBlur.GetWidth(), 1.f / BilateralBlur.GetHeight());
	SSAOBlurParam.BlurRadius = BlurRadius;

	SSAOBlurConstantBufferParam.Update(0, &SSAOBlurParam);
}

void FScreenSpaceAmbientOcclusion::BuildDescriptors()
{
	if (GeometryMap && RenderLayer)
	{
		NormalBuffer.SetSRVOffset(GetNormalBufferSRVOffset());
		NormalBuffer.SetRTVOffset(GetNormalBufferRTVOffset());
		NormalBuffer.SetDepthSRVOffset(GetDepthBufferSRVOffset());
		NormalBuffer.SetDepthRTVOffset(GetDepthBufferRTVOffset());
		NormalBuffer.BuildDescriptors();
		NormalBuffer.BuildRenderTargetRTV();
		NormalBuffer.BuildSRVDescriptors();
		NormalBuffer.BuildRTVDescriptors();

		NoiseBuffer.SetSRVOffset(GetNoiseBufferSRVOffset());
		NoiseBuffer.BuildDescriptors();
		NoiseBuffer.BuildRenderTargetRTV();
		NoiseBuffer.BuildSRVDescriptors();
		NoiseBuffer.BuildRTVDescriptors();

		AmbientBuffer.SetSRVOffset(GetAmbientBufferSRVOffset());
		AmbientBuffer.SetRTVOffset(GetAmbientBufferRTVOffset());
		AmbientBuffer.BuildDescriptors();
		AmbientBuffer.BuildRenderTargetRTV();
		AmbientBuffer.BuildSRVDescriptors();
		AmbientBuffer.BuildRTVDescriptors();

		// 初始化双边模糊
		// バイラテラルブラーを初期化
		BilateralBlur.SetSRVOffset(GetBilateralBlurSRVOffset());
		BilateralBlur.SetRTVOffset(GetBilateralBlurRTVOffset());
		BilateralBlur.BuildDescriptors();
		BilateralBlur.BuildRenderTargetRTV();
		BilateralBlur.BuildSRVDescriptors();
		BilateralBlur.BuildRTVDescriptors();
	}
}

void FScreenSpaceAmbientOcclusion::Build()
{
	// 构建根签名
	// ルートシグネチャを構築
	DirectXRootSignature.BuildRootSignature();

	// 构建SSAOView常量缓冲区
	// SSAOView定数バッファを構築
	BuildSSAOViewConstantBuffer();

	// 构建模糊常量缓冲区
	// ブラー定数バッファを構築
	BuildSSAOBlurParamConstantBuffer();

	// 绑定BuildPSO
	// Build PSOをバインド
	BindBuildPSO();

	// 构建模糊权重
	// ブラーの重みを構築
	BuildBlurWeights(2.5f);
}

void FScreenSpaceAmbientOcclusion::BindBuildPSO()
{
	if (RenderLayer)
	{
		if (std::shared_ptr<FRenderLayer> SSAOLayer = RenderLayer->FindByRenderLayer(EMeshRenderLayerType::RENDERLAYER_SSAO))
		{
			SSAOLayer->BuildPSODelegate.Bind(this, &FScreenSpaceAmbientOcclusion::BuildPSO);
		}

		if (std::shared_ptr<FRenderLayer> SSAOLayer = RenderLayer->FindByRenderLayer(EMeshRenderLayerType::RENDERLAYER_SSAO_BILATERAL_BLUR))
		{
			SSAOLayer->BuildPSODelegate.Bind(this, &FScreenSpaceAmbientOcclusion::BuildPSO);
		}
	}
}

void FScreenSpaceAmbientOcclusion::BuildPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC& OutPSODesc)
{
	OutPSODesc.pRootSignature = DirectXRootSignature.GetRootSignature();
}

void FScreenSpaceAmbientOcclusion::BuildSSAOViewConstantBuffer()
{
	// FSSAOViewportTransformation
	// 创建常量缓冲区
	// 定数バッファを作成
	SSAOViewConstantBufferViews.CreateConstant(sizeof(FSSAOViewportTransformation), 1);
}

void FScreenSpaceAmbientOcclusion::BuildSSAOBlurParamConstantBuffer()
{
	// 构建双边模糊常量缓冲区
	// バイラテラルブラー用の定数バッファを構築
	SSAOBlurConstantBufferParam.CreateConstant(sizeof(FSSAOBlurParam), 1);
}

void FScreenSpaceAmbientOcclusion::SaveToSSAOBuffer()
{
	// 将 SSAO 结果写回默认管线使用的贴图槽位
	// SSAO の結果をデフォルトパイプラインが参照するテクスチャスロットへ書き戻す
	GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
		9,
		AmbientBuffer.GetRenderTarget()->GetGPUSRVOffset());
}

void FScreenSpaceAmbientOcclusion::DrawBlur(float DeltaTime, bool bHorizontal)
{
	// 1.リソース
	// 2.SRV
	// 3.RTV

	// 通知Shader
	// シェーダーに通知
	SetRoot32BitConstant(bHorizontal);

	ID3D12Resource* InDrawResources = GetDrawResources(bHorizontal);
	CD3DX12_CPU_DESCRIPTOR_HANDLE* InDrawResourcesRTV = GetDrawRTVResources(bHorizontal);

	// 转换资源状态
	// リソース状態を変換
	CD3DX12_RESOURCE_BARRIER ResourceBarrierPresent = CD3DX12_RESOURCE_BARRIER::Transition(
		InDrawResources,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
	GetGraphicsCommandList()->ResourceBarrier(1, &ResourceBarrierPresent);

	const float ClearColor[] = { 1.f,1.f,1.f,1.f };
	GetGraphicsCommandList()->ClearRenderTargetView(
		*InDrawResourcesRTV,
		ClearColor, 0, nullptr);

	GetGraphicsCommandList()->OMSetRenderTargets(1,
		InDrawResourcesRTV,
		true, nullptr);

	// 绑定接受的缓冲区
	// 受け取るバッファをバインド
	GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
		6,
		*GetDrawSRVResources(bHorizontal));

	// 渲染SSAOPSO
	// SSAO PSOを描画
	RenderLayer->Draw(RENDERLAYER_SSAO_BILATERAL_BLUR, DeltaTime);

	CD3DX12_RESOURCE_BARRIER ResourceBarrierPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
		InDrawResources,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
	GetGraphicsCommandList()->ResourceBarrier(1, &ResourceBarrierPresentRenderTarget);
}

UINT FScreenSpaceAmbientOcclusion::GetDepthBufferSRVOffset() const
{
	return	GeometryMap->GetDrawTexture2DResourcesNumber() + //Texture2D
		GeometryMap->GetDrawCubeMapResourcesNumber() + //静态Cube贴图 背景 天空球  // 静的キューブマップ（背景・スカイボックス）
		1 + //动态Cube贴图 反射          // 動的キューブマップ（反射）
		1 + //Shadow 直射灯 聚光灯       // シャドウ（平行光・スポットライト）
		1 + //ShadowCubeMap 点光源       // シャドウキューブマップ（ポイントライト）
		1 + //UI
		1;  //Nor
}

UINT FScreenSpaceAmbientOcclusion::GetDepthBufferRTVOffset() const
{
	return	FEngineRenderConfig::GetRenderConfig()->SwapChainCount +
		6 +
		6 +
		1;
}

UINT FScreenSpaceAmbientOcclusion::GetNormalBufferSRVOffset() const
{
	return  GeometryMap->GetDrawTexture2DResourcesNumber() + //Texture2D
		GeometryMap->GetDrawCubeMapResourcesNumber() + //静态Cube贴图 背景 天空球  // 静的キューブマップ（背景・スカイボックス）
		1 + //动态Cube贴图 反射                     // 動的キューブマップ（反射）
		1 + //Shadow 直射灯 聚光灯                  // シャドウ（平行光・スポットライト）
		1 + //ShadowCubeMap 点光源的                // シャドウキューブマップ（ポイントライト）
		1;//UI
}

UINT FScreenSpaceAmbientOcclusion::GetNormalBufferRTVOffset() const
{
	return	FEngineRenderConfig::GetRenderConfig()->SwapChainCount +//交换链     // スワップチェーン
		6 +//反射的CubeMap RTV            // 反射用CubeMap RTV                          
		6; //ShadowCubeMap RTV Point Light
}

UINT FScreenSpaceAmbientOcclusion::GetNoiseBufferSRVOffset() const
{
	return  GeometryMap->GetDrawTexture2DResourcesNumber() + //Texture2D
		GeometryMap->GetDrawCubeMapResourcesNumber() + //静态Cube贴图 背景 天空球  // 静的キューブマップ（背景・スカイボックス）
		1 + //动态Cube贴图 反射               // 動的キューブマップ（反射）
		1 + //Shadow 直射灯 聚光灯            // シャドウ（平行光・スポットライト）
		1 + //ShadowCubeMap 点光源的          // シャドウキューブマップ（ポイントライト）
		1 + //UI
		1 + //法线                            // 法線
		1; //深度                             //Depth
}

UINT FScreenSpaceAmbientOcclusion::GetAmbientBufferSRVOffset() const
{
	return	GeometryMap->GetDrawTexture2DResourcesNumber() + //Texture2D
		GeometryMap->GetDrawCubeMapResourcesNumber() + //静态Cube贴图 背景 天空球  // 静的キューブマップ（背景・スカイボックス）
		1 + //动态Cube贴图 反射               // 動的キューブマップ（反射）
		1 + //Shadow 直射灯 聚光灯            // シャドウ（平行光・スポットライト）
		1 + //ShadowCubeMap 点光源的          // シャドウキューブマップ（ポイントライト）
		1 + //UI
		1 + //法线                            // 法線
		1 +//深度                             //Depth
		1;  //Noise
}

UINT FScreenSpaceAmbientOcclusion::GetAmbientBufferRTVOffset() const
{
	return  FEngineRenderConfig::GetRenderConfig()->SwapChainCount +//交换链    // スワップチェーン
		6 +//反射的CubeMap RTV                     // 反射用CubeMap RTV                       
		6 +//ShadowCubeMap RTV Point Light
		2; //法线 + 深度                            // 法線 + Depth
}

UINT FScreenSpaceAmbientOcclusion::GetBilateralBlurSRVOffset() const
{
	return	GeometryMap->GetDrawTexture2DResourcesNumber() + //Texture2D
		GeometryMap->GetDrawCubeMapResourcesNumber() + //静态Cube贴图 背景 天空球  // 静的キューブマップ（背景・スカイボックス）
		1 + //动态Cube贴图 反射               // 動的キューブマップ（反射）
		1 + //Shadow 直射灯 聚光灯            // シャドウ（平行光・スポットライト）
		1 + //ShadowCubeMap 点光源的          // シャドウキューブマップ（ポイントライト）
		1 + //UI
		1 + //法线                            // 法線
		1 + //深度                            //Depth
		1 + //Noise
		1;  //SSAO
}

UINT FScreenSpaceAmbientOcclusion::GetBilateralBlurRTVOffset() const
{
	return  FEngineRenderConfig::GetRenderConfig()->SwapChainCount +//交换链    // スワップチェーン
		6 +//反射的CubeMap RTV                        // 反射用CubeMap RTV             
		6 +//ShadowCubeMap RTV Point Light
		2 +//法线 + 深度                              // 法線 + Depth
		1; //SSAO
}
