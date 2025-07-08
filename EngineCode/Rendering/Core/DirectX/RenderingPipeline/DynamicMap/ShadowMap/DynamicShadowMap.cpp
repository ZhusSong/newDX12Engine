#include "DynamicShadowMap.h"
#include "../../RenderTarget/ShadowMapRenderTarget.h"
#include "../../Geometry/GeometryMap.h"
#include "../../RenderTarget/ShadowMapRenderTarget.h"
#include "../../RenderLayer/RenderLayerManager.h"
#include "../../../../../../Config/EngineRenderConfig.h"
#include "../../../.././../../Core/Viewport/ClientViewport.h"
#include "../../../../../../Component/Mesh/Core/MeshComponentType.h"
#include "../../../../../../Component/Mesh/Core/MeshComponent.h"
#include "../../../../../../Component/Light/Core/LightComponent.h"
#include "../../../../../../Manager/LightManager.h"
#include "../../../../../../Component/Light/Core/LightConstantBuffer.h"
#include "../../../../../../Component/Light/SpotLightComponent.h"
#include "../../../../../../Component/Light/ParallelLightComponent.h"
#include "../../../../../../Component/Light/PointLightComponent.h"

FDynamicShadowMap::FDynamicShadowMap()
	:Super()
{
	CreateRenderTarget<FShadowMapRenderTarget>();
}
void FDynamicShadowMap::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	Super::UpdateCalculations(DeltaTime, ViewportInfo);

	//更新视口
	if (ShadowViewport)
	{
		for (size_t i = 0; i < GetLightManager()->GetLights().size(); i++)
		{
			if (CLightComponent* InLightComponent = GetLightManager()->GetLights()[i])
			{
				FViewportInfo ShadowViewportInfo;
				GetViewportMatrix(ShadowViewportInfo.ViewMatrix, ShadowViewportInfo.ProjectMatrix);
				ShadowViewportInfo.ViewPosition =
					XMFLOAT4(
						ShadowViewport->GetPosition().x,
						ShadowViewport->GetPosition().y,
						ShadowViewport->GetPosition().z,
						1.f);

				GeometryMap->UpdateCalculationsViewport(
					DeltaTime,
					ShadowViewportInfo,
					GeometryMap->GetDynamicReflectionViewportNum() + //动态反射的摄像机
					1);//主视口
			}
		}
	}
}


void FDynamicShadowMap::Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer)
{
	Super::Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);
}

void FDynamicShadowMap::PreDraw(float DeltaTime)
{
	Super::PreDraw(DeltaTime);
}

void FDynamicShadowMap::Draw(float DeltaTime)
{
	Super::Draw(DeltaTime);

	for (size_t i = 0; i < GetLightManager()->GetLights().size(); i++)
	{
		if (CLightComponent* InLightComponent = GetLightManager()->GetLights()[i])
		{
			if (FShadowMapRenderTarget* InRenderTarget = dynamic_cast<FShadowMapRenderTarget*>(RenderTarget.get()))
			{
				ELightType InLightType = InLightComponent->GetLightType();
				if (InLightType != ELightType::PointLight)
				{
					CD3DX12_RESOURCE_BARRIER ResourceBarrierPresent = CD3DX12_RESOURCE_BARRIER::Transition(
						RenderTarget->GetRenderTarget(),
						D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

					GetGraphicsCommandList()->ResourceBarrier(1, &ResourceBarrierPresent);

					// 需要每帧执行
					// 绑定矩形框
					auto RenderTargetViewport = InRenderTarget->GetViewport();
					auto RenderTargetScissorRect = InRenderTarget->GetScissorRect();
					GetGraphicsCommandList()->RSSetViewports(1, &RenderTargetViewport);
					GetGraphicsCommandList()->RSSetScissorRects(1, &RenderTargetScissorRect);

					UINT CBVSize = GeometryMap->ViewportConstantBufferViews.GetConstantBufferByteSize();

					// 清除深度模板缓冲区
					GetGraphicsCommandList()->ClearDepthStencilView(
						InRenderTarget->DSVDes,
						D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
						1.f, 0, 0, NULL);

					// 输出的合并阶段
					GetGraphicsCommandList()->OMSetRenderTargets(0,
						nullptr,
						false,
						&InRenderTarget->DSVDes);

					auto ViewprotAddr = GeometryMap->ViewportConstantBufferViews.GetBuffer()->GetGPUVirtualAddress();
					ViewprotAddr += (
						1 + //主摄像机
						GeometryMap->GetDynamicReflectionViewportNum()) //反射摄像机
						* CBVSize;

					GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(1, ViewprotAddr);

					DrawShadowMapTexture(DeltaTime);

					switch (InLightType)
					{
					case DirectionalLight:
						RenderLayer->ResetPSO(RENDERLAYER_SHADOW_RENDER, EPipelineState::OrthogonalShadow);
						break;
					case PointLight:
						RenderLayer->ResetPSO(RENDERLAYER_SHADOW_RENDER, EPipelineState::PerspectiveShadow);
						break;
					case SpotLight:
						RenderLayer->ResetPSO(RENDERLAYER_SHADOW_RENDER, EPipelineState::PerspectiveShadow);
						break;
					}

					RenderLayer->DrawMesh(DeltaTime, RENDERLAYER_OPAQUE, ERenderingConditions::RC_Shadow);
					RenderLayer->DrawMesh(DeltaTime, RENDERLAYER_TRANSPARENT, ERenderingConditions::RC_Shadow);
					RenderLayer->DrawMesh(DeltaTime, RENDERLAYER_OPAQUE_REFLECTOR, ERenderingConditions::RC_Shadow);

					CD3DX12_RESOURCE_BARRIER ResourceBarrierPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
						RenderTarget->GetRenderTarget(),
						D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);

					GetGraphicsCommandList()->ResourceBarrier(1, &ResourceBarrierPresentRenderTarget);
				}
			}
		}
	}
}


void FDynamicShadowMap::GetViewportMatrix(XMFLOAT4X4& OutViewMatrix, XMFLOAT4X4& OutProjectMatrix)
{
	if (ShadowViewport)
	{
		OutViewMatrix = ShadowViewport->ViewMatrix;
		OutProjectMatrix = ShadowViewport->ProjectMatrix;
	}
}

void FDynamicShadowMap::DrawShadowMapTexture(float DeltaTime)
{
	GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(7, RenderTarget->GetGPUSRVOffset());
}

void FDynamicShadowMap::SetViewportPosition(const fvector_3d& InPosition)
{
	ShadowViewport->SetPosition(XMFLOAT3(InPosition.x, InPosition.y, InPosition.z));
}

void FDynamicShadowMap::SetViewportRotation(const fvector_3d& InRotation)
{
	ShadowViewport->SetRotation(InRotation);
}

void FDynamicShadowMap::BuildViewMatrix(float DeltaTime)
{
	ShadowViewport->BuildViewMatrix(DeltaTime);
}

void FDynamicShadowMap::BuildParallelLightMatrix(
	const fvector_3d& InDirection,
	const fvector_3d& InTargetPosition,
	float InRadius)
{
	// ShadowViewport
	// 构建ViewMatrix
	fvector_3d ViewPosition = (InDirection * -InRadius);
	ShadowViewport->SetPosition(XMFLOAT3(ViewPosition.x, ViewPosition.y, ViewPosition.z));
	ShadowViewport->FaceTarget(ViewPosition, InTargetPosition, fvector_3d(0.f, 1.f, 0.f));

	BuildViewMatrix(0.3f);

	// 构建ProjMatrix
	ShadowViewport->BuildOrthographicOffCenterLHMatrix(InRadius, InTargetPosition);
}

void FDynamicShadowMap::BuildSpotLightMatrix(
	const fvector_3d& InDirection,
	const fvector_3d& InPosition, float InRadius)
{
	// ShadowViewport
	// 构建ViewMatrix
	fvector_3d ViewPosition = (InDirection * -InRadius);
	ShadowViewport->SetPosition(XMFLOAT3(InPosition.x, InPosition.y, InPosition.z));
	ShadowViewport->FaceTarget(InPosition, ViewPosition, fvector_3d(0.f, 1.f, 0.f));

	BuildViewMatrix(0.3f);

	// 构建ProjMatrix
	ShadowViewport->SetFrustum(0.9f * XM_PI, 1.f, 0.1f, InRadius);
}

void FDynamicShadowMap::BuildViewport(const fvector_3d& InCenterPoint)
{
	ShadowViewport = CreateObject<GClientViewport>(new GClientViewport());
	ShadowViewport->SetPosition(XMFLOAT3(InCenterPoint.x, InCenterPoint.y, InCenterPoint.z));
	ShadowViewport->FaceTarget(InCenterPoint, fvector_3d(10.f), fvector_3d(0.f, 1.f, 0.f));
	ShadowViewport->SetFrustum(0.5f * XM_PI, 1.f, 0.1f, 100.f);
	BuildViewMatrix(0.3f);
}

void FDynamicShadowMap::BuildDepthStencilDescriptor()
{  
	// 确保描述符堆已创建且有效
	if (!GetDSVHeap())
	{
		Engine_Log("需要先创建描述符堆");
		// 需要先创建描述符堆
		return;
	}
	UINT DescriptorHandleIncrementSize = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	if (FShadowMapRenderTarget* InRenderTarget = dynamic_cast<FShadowMapRenderTarget*>(RenderTarget.get()))
	{

		InRenderTarget->DSVDes = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			GetDSVHeap()->GetCPUDescriptorHandleForHeapStart(),
			1 + //主视口的 DSV
			1, //CubeMap DSV
			DescriptorHandleIncrementSize);
	}
}

void FDynamicShadowMap::Init(int InWidth, int InHeight)
{
	Width = InWidth;
	Height = InHeight;
}

void FDynamicShadowMap::BuildRenderTargetSRV()
{
	UINT CBVDescriptorSize = GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();

	auto CPUSRVDesHeapStart = GeometryMap->GetHeap()->GetCPUDescriptorHandleForHeapStart();
	auto GPUSRVDesHeapStart = GeometryMap->GetHeap()->GetGPUDescriptorHandleForHeapStart();

	int ShadowMapOffset =
		GeometryMap->GetDrawTexture2DResourcesNumber() 
		+ GeometryMap->GetDrawCubeMapResourcesNumber()
		+ 1;//Cubemap

	if (FShadowMapRenderTarget* InRenderTarget = dynamic_cast<FShadowMapRenderTarget*>(RenderTarget.get()))
	{
		// 创建Shadow常量缓冲区
		InRenderTarget->CPUShaderResourceView =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(CPUSRVDesHeapStart,
				ShadowMapOffset,//
				CBVDescriptorSize);

		InRenderTarget->GPUShaderResourceView =
			CD3DX12_GPU_DESCRIPTOR_HANDLE(GPUSRVDesHeapStart,
				ShadowMapOffset,//
				CBVDescriptorSize);
	}
}

void FDynamicShadowMap::BuildRenderTargetDescriptor()
{
	BuildRenderTargetSRV();

	RenderTarget->Init(Width, Height, DXGI_FORMAT_D24_UNORM_S8_UINT);
}