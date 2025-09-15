#include "ScreenSpaceAmbientOcclusion.h"
#include "SSAOType.h"
#include "../../Geometry/GeometryMap.h"
#include "../../RenderLayer/RenderLayerManager.h"
#include "../../RenderTarget/BufferRenderTarget.h"
#include "../../RenderBuffer/DepthBuffer.h"
#include "../../../../../../Component/Mesh/Core/MeshComponentType.h"
#include "../../../../../../Core/Viewport/ViewportInfo.h"

FScreenSpaceAmbientOcclusion::FScreenSpaceAmbientOcclusion()
{
	RenderLayer = NULL;
}

void FScreenSpaceAmbientOcclusion::Init(
	FGeometryMap* InGeometryMap, 
	FDirectXPipelineState* InDirectXPipelineState,
	FRenderLayerManager* InRenderLayer)
{
	NormalBuffer.Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);
	AmbientBuffer.Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);

	RenderLayer = InRenderLayer;
	GeometryMap = InGeometryMap;
}

void FScreenSpaceAmbientOcclusion::Init(int InWidth, int InHeight)
{
	NormalBuffer.Init(InWidth, InHeight); 
	AmbientBuffer.Init(InWidth, InHeight);
}
void FScreenSpaceAmbientOcclusion::Build()
{
	// 构建根签名
	DirectXRootSignature.BuildRootSignature();

	// 构建视口常量缓冲区
	BuildSSAOViewConstantBuffer();

	// 构建PSO
	BindBuildPSO();
}

void FScreenSpaceAmbientOcclusion::BindBuildPSO()
{
	if (RenderLayer)
	{
		if (std::shared_ptr<FRenderLayer> SSAOLayer = RenderLayer->FindByRenderLayer(EMeshRenderLayerType::RENDERLAYER_SSAO))
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
	SSAOViewConstantBufferViews.CreateConstant(sizeof(FSSAOViewportTransformation), 1);
}

void FScreenSpaceAmbientOcclusion::SaveToSSAOBuffer()
{
}

void FScreenSpaceAmbientOcclusion::BuildDepthBuffer()
{
}

void FScreenSpaceAmbientOcclusion::Draw(float DeltaTime)
{
	NormalBuffer.Draw(DeltaTime);
	AmbientBuffer.Draw(DeltaTime);
	
	// 构建SSAO
	// 设置根签名
	DirectXRootSignature.PreDraw(DeltaTime);

	// 主SSAO渲染
	if (FBufferRenderTarget* InRenderTarget = dynamic_cast<FBufferRenderTarget*>(AmbientBuffer.GetRenderTarget().get()))
	{
		auto RenderTargetViewport = InRenderTarget->GetViewport();
		auto RenderTargetScissorRect = InRenderTarget->GetScissorRect();

		GetGraphicsCommandList()->RSSetViewports(1, &RenderTargetViewport);
		GetGraphicsCommandList()->RSSetScissorRects(1, &RenderTargetScissorRect);

		
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

		// 绑定SSAO常量缓冲区
		GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(
			0,
			SSAOViewConstantBufferViews.GetBuffer()->GetGPUVirtualAddress());

		// 法线
		if (std::shared_ptr<FRenderTarget> NormalRenderTarget = NormalBuffer.GetRenderTarget())
		{
			GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
				1,
				NormalRenderTarget->GetGPUSRVOffset());
		}

		// 深度
		GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
			2,
			DepthBufferRenderTarget->GetGPUSRVOffset());

		// 渲染SSAOPSO
		RenderLayer->Draw(RENDERLAYER_SSAO, DeltaTime);

		CD3DX12_RESOURCE_BARRIER ResourceBarrierPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
			InRenderTarget->GetRenderTarget(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
		GetGraphicsCommandList()->ResourceBarrier(1, &ResourceBarrierPresentRenderTarget);
	}
}


void FScreenSpaceAmbientOcclusion::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	NormalBuffer.UpdateCalculations(DeltaTime, ViewportInfo);
	AmbientBuffer.UpdateCalculations(DeltaTime, ViewportInfo);

	DrawViewConstantBufferViews(DeltaTime, ViewportInfo);

}

void FScreenSpaceAmbientOcclusion::DrawViewConstantBufferViews(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	FSSAOViewportTransformation SSAOViewportTransformation;

	// 投影矩阵
	SSAOViewportTransformation.ProjectionMatrix = ViewportInfo.ProjectMatrix;

	// 逆矩阵
	XMMATRIX ProjectMatrixRIX = XMLoadFloat4x4(&ViewportInfo.ProjectMatrix);

	XMVECTOR ProjectDeterminant = XMMatrixDeterminant(ProjectMatrixRIX);
	XMMATRIX InversiveProjectionMatrixRIX = XMMatrixInverse(&ProjectDeterminant, ProjectMatrixRIX);
	XMStoreFloat4x4(&SSAOViewportTransformation.InversiveProjectionMatrix, XMMatrixTranspose(InversiveProjectionMatrixRIX));

	//[-1.1] =>[0,1] 贴图空间
	XMMATRIX HalLambertMatrix(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX TexProjectionMatrixRIX = XMMatrixMultiply(ProjectMatrixRIX, HalLambertMatrix);
	XMStoreFloat4x4(&SSAOViewportTransformation.TexProjectionMatrix, XMMatrixTranspose(TexProjectionMatrixRIX));

	SSAOViewConstantBufferViews.Update(0, &SSAOViewportTransformation);
}


void FScreenSpaceAmbientOcclusion::BuildDescriptors()
{
	NormalBuffer.BuildDescriptors();
	NormalBuffer.BuildRenderTargetRTV();
	NormalBuffer.BuildSRVDescriptors();
	NormalBuffer.BuildRTVDescriptors();

	AmbientBuffer.BuildDescriptors();
	AmbientBuffer.BuildRenderTargetRTV();
	AmbientBuffer.BuildSRVDescriptors();
	AmbientBuffer.BuildRTVDescriptors();
}

