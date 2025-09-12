#include "ScreenSpaceAmbientOcclusion.h"
#include "../../Geometry/GeometryMap.h"
#include "SSAOType.h"
#include "../../../../../../Core/Viewport/ViewportInfo.h"
#include "../../RenderLayer/RenderLayerManager.h"
#include "../../../../../../Component/Mesh/Core/MeshComponentType.h"

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

