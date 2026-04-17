#include "DynamicShadowCubeMap.h"
#include "../../Geometry/GeometryMap.h"
#include "../../PipelineState/DirectXPipelineState.h"
#include "../../../.././../../Core/Viewport/ClientViewport.h"
#include "../../RenderLayer/RenderLayerManager.h"
#include "../../../../../../Config/EngineRenderConfig.h"
#include "../../../../../../Component/Mesh/Core/MeshComponentType.h"
#include "../../../../../../Component/Mesh/Core/MeshComponent.h"
#include "../../../../../../Manager/LightManager.h"
#include "../../../../../../Component/Light/PointLightComponent.h"

FDynamicShadowCubeMap::FDynamicShadowCubeMap()
	:Super()
{

}

void FDynamicShadowCubeMap::OnResetSize(int InWidth, int InHeight)
{
}


void FDynamicShadowCubeMap::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	if (CubeMapViewport.size() == 6)
	{
		int Index = 0;
		for (int i = 0; i < GetLightManager()->GetLights().size(); i++)
		{
			CLightComponent* Tmp = GetLightManager()->GetLights()[i];
			if (Tmp->GetLightType() == ELightType::PointLight)
			{
				XMFLOAT3 F3 = Tmp->GetPosition();
				SetViewportPosition(fvector_3d(F3.x, F3.y, F3.z));

				for (size_t j = 0; j < 6; j++)
				{
					FViewportInfo MyViewportInfo;

					XMFLOAT3 ViewPosition = CubeMapViewport[j]->GetPosition();
					MyViewportInfo.ViewPosition = XMFLOAT4(ViewPosition.x, ViewPosition.y, ViewPosition.z, 1.f);
					MyViewportInfo.ViewMatrix = CubeMapViewport[j]->ViewMatrix;
					MyViewportInfo.ProjectMatrix = CubeMapViewport[j]->ProjectMatrix;

					GeometryMap->UpdateCalculationsViewport(DeltaTime, MyViewportInfo,
						1 +//给主视口                        	// メインビュー用
						GeometryMap->GetDynamicReflectionViewportNum() + //给Shadow动态摄像机  // Shadow用の動的カメラ
						1 +//给Shadow摄像机                     // Shadow用カメラ
						j + Index * 6 //给动态摄像机            // 動的カメラ用
					); 
				}

				Index++;
			}
		}
	}
}


void FDynamicShadowCubeMap::Init(
	FGeometryMap* InGeometryMap,
	FDirectXPipelineState* InDirectXPipelineState,
	FRenderLayerManager* InRenderLayer)
{
	Super::Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);
}

void FDynamicShadowCubeMap::PreDraw(float DeltaTime)
{
	if (FCubeMapRenderTarget* InRenderTarget = dynamic_cast<FCubeMapRenderTarget*>(RenderTarget.get()))
	{
		int Index = 0;
		for (int j = 0; j < GetLightManager()->GetLights().size(); j++)
		{
			CLightComponent* Tmp = GetLightManager()->GetLights()[j];
			if (Tmp->GetLightType() == ELightType::PointLight)
			{
				// 转换资源状态
				// リソース状態を変換
				CD3DX12_RESOURCE_BARRIER ResourceBarrierPresent = CD3DX12_RESOURCE_BARRIER::Transition(
					InRenderTarget->GetRenderTarget(),
					D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);

				GetGraphicsCommandList()->ResourceBarrier(1, &ResourceBarrierPresent);

				CD3DX12_RESOURCE_BARRIER DepthStencilToWrite = CD3DX12_RESOURCE_BARRIER::Transition(
					DepthStencilBuffer.Get(),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					D3D12_RESOURCE_STATE_DEPTH_WRITE);

				GetGraphicsCommandList()->ResourceBarrier(1, &DepthStencilToWrite);

				// 需要每帧执行
				// 绑定矩形框
				// 毎フレーム実行が必要
				// 矩形ボックスをバインド
				auto RenderTargetViewport = InRenderTarget->GetViewport();
				auto RenderTargetScissorRect = InRenderTarget->GetScissorRect();
				GetGraphicsCommandList()->RSSetViewports(1, &RenderTargetViewport);
				GetGraphicsCommandList()->RSSetScissorRects(1, &RenderTargetScissorRect);

				UINT CBVSize = GeometryMap->GetViewportConstantBufferByteSize();
				for (size_t i = 0; i < 6; i++)
				{
					// 清除画布
					// 描画内容をクリア
					GetGraphicsCommandList()->ClearRenderTargetView(
						InRenderTarget->GetCPURenderTargetView(i),
						DirectX::Colors::White,
						0, nullptr);

					// 清除深度模板缓冲区
					// 深度バッファーをクリア
					GetGraphicsCommandList()->ClearDepthStencilView(
						DSVDes,
						D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
						1.f, 0, 0, NULL);

					// 输出的合并阶段
					// 出力のマージフェーズ
					GetGraphicsCommandList()->OMSetRenderTargets(1,
						&InRenderTarget->GetCPURenderTargetView(i),
						true,
						&DSVDes);

					// 更新6个摄像机 绑定6个摄像机
					// 6つのカメラを更新し、6つのカメラをバインド
					auto ViewprotAddr = GeometryMap->ViewportGPUVirtualAddress();
					ViewprotAddr += (
						1 + //主摄像机                // メインカメラ
						GeometryMap->GetDynamicReflectionViewportNum() + //CubeMap 反射
						1 + //Shadow 平行光 聚光灯    //シャドウ 平行光 スポットライト 
						i + Index * 6 //
						) * CBVSize;

					GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(1, ViewprotAddr);

					RenderLayer->ResetPSO(RENDERLAYER_SHADOW_RENDER, EPipelineState::OmnidirectionalShadow);

					// 各类层级渲染
					// 各レイヤーのレンダリング
					RenderLayer->DrawMesh(DeltaTime, RENDERLAYER_OPAQUE, ERenderingConditions::RC_Shadow);
					RenderLayer->DrawMesh(DeltaTime, RENDERLAYER_TRANSPARENT, ERenderingConditions::RC_Shadow);
					RenderLayer->DrawMesh(DeltaTime, RENDERLAYER_OPAQUE_REFLECTOR, ERenderingConditions::RC_Shadow);
				}

				CD3DX12_RESOURCE_BARRIER ResourceBarrierPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
					RenderTarget->GetRenderTarget(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);

				GetGraphicsCommandList()->ResourceBarrier(1, &ResourceBarrierPresentRenderTarget);

				CD3DX12_RESOURCE_BARRIER DepthStencilToRead = CD3DX12_RESOURCE_BARRIER::Transition(
					DepthStencilBuffer.Get(),
					D3D12_RESOURCE_STATE_DEPTH_WRITE,
					D3D12_RESOURCE_STATE_GENERIC_READ);

				GetGraphicsCommandList()->ResourceBarrier(1, &DepthStencilToRead);

			
				// 绘制到ShadowCubeMap
				// ShadowCubeMapに描画
				GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(8, InRenderTarget->GetGPUSRVOffset());

				Index++;
			}
		}
	}
}

void FDynamicShadowCubeMap::Draw(float DeltaTime)
{

}

// 设置DSV偏移
// DSVオフセットを設定
void FDynamicShadowCubeMap::BuildDepthStencilDescriptor()
{
	UINT DescriptorHandleIncrementSize = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	DSVDes = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		GetDSVHeap()->GetCPUDescriptorHandleForHeapStart(),
		3,
		DescriptorHandleIncrementSize);
}


// RenderTarget 
void FDynamicShadowCubeMap::BuildRenderTargetRTV()
{
	UINT RTVDescriptorSize = GetDescriptorHandleIncrementSizeByRTV();

	// RTV的起始
	// RTVの開始
	auto RTVDesHeapStart = GetRTVHeap()->GetCPUDescriptorHandleForHeapStart();

	if (FCubeMapRenderTarget* InRenderTarget = dynamic_cast<FCubeMapRenderTarget*>(RenderTarget.get()))
	{
		// 偏移的地址记录
		// オフセットアドレスの記録
		for (size_t i = 0; i < 6; i++)
		{
			InRenderTarget->GetCPURenderTargetView(i) = CD3DX12_CPU_DESCRIPTOR_HANDLE(
				RTVDesHeapStart,
				FEngineRenderConfig::GetRenderConfig()->SwapChainCount
				+ 6  //反射 CubeMap的摄像机   //反射用CubeMapのカメラ
				+ i,
				RTVDescriptorSize);
		}
	}
}

void FDynamicShadowCubeMap::BuildRenderTargetSRV()
{
	UINT CBVDescriptorSize = GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();

	auto CPUSRVDesHeapStart = GeometryMap->GetHeap()->GetCPUDescriptorHandleForHeapStart();
	auto GPUSRVDesHeapStart = GeometryMap->GetHeap()->GetGPUDescriptorHandleForHeapStart();

	int Offset =
		GeometryMap->GetDrawTexture2DResourcesNumber() +
		GeometryMap->GetDrawCubeMapResourcesNumber() +
		1 + //反射cubemap          //反射用CubeMap
		1;//shadow

	if (FCubeMapRenderTarget* InRenderTarget = dynamic_cast<FCubeMapRenderTarget*>(RenderTarget.get()))
	{
		InRenderTarget->GetCPUSRVOffset() =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(CPUSRVDesHeapStart,
				Offset,
				CBVDescriptorSize);

		InRenderTarget->GetGPUSRVOffset() =
			CD3DX12_GPU_DESCRIPTOR_HANDLE(GPUSRVDesHeapStart,
				Offset,
				CBVDescriptorSize);
	}
}
FDynamicShadowCubeMap::FTmpViewportCapture::FTmpViewportCapture(const fvector_3d& InCenterPoint)
{
	BuildViewportCapture(InCenterPoint);
}
