#include "RenderLayer.h"
#include "../RenderLayerManager.h"
#include "../../Geometry/GeometryMap.h"
#include "../../PipelineState/DirectXPipelineState.h"
#include "../../../../../../Mesh/Core/Material/Material.h"
#include "../../../../../../Component/Mesh/Core/MeshComponent.h"
#include "../../../../../../Mesh/Core/ObjectTransform.h"
#include "../../../../../../Core/Viewport/ViewportInfo.h"

FRenderLayer::FRenderLayer()
	:RenderPriority(0)
{

}

void FRenderLayer::RegisterRenderLayer()
{
	FRenderLayerManager::RenderLayers.push_back(this->shared_from_this());
}

void FRenderLayer::BuildShaderMacro(std::vector<ShaderType::FShaderMacro>& InMacro)
{
	{
		ShaderType::FShaderMacro ShaderMacro;

		char TextureNumBuff[10] = { 0 };
		ShaderMacro.Name = "TEXTURE2D_MAP_NUM";
		ShaderMacro.Definition = _itoa(GeometryMap->GetDrawTexture2DResourcesNumber(), TextureNumBuff, 10);

		InMacro.push_back(ShaderMacro);
	}

	{
		ShaderType::FShaderMacro ShaderMacro;

		char TextureNumBuff[10] = { 0 };
		ShaderMacro.Name = "CUBE_MAP_NUM";
		ShaderMacro.Definition = _itoa(GeometryMap->GetDrawCubeMapResourcesNumber(), TextureNumBuff, 10);

		InMacro.push_back(ShaderMacro);
	}

	{
		ShaderType::FShaderMacro ShaderMacro;
		ShaderMacro.Name = "START_UP_FOG";
		ShaderMacro.Definition = GeometryMap->IsStartUPFog() ? "1" : "0";

		InMacro.push_back(ShaderMacro);
	}
}

void FRenderLayer::Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState)
{
	GeometryMap = InGeometryMap;
	DirectXPipelineState = InDirectXPipelineState;
}

void FRenderLayer::PreDraw(float DeltaTime)
{

}

void FRenderLayer::Draw(float DeltaTime)
{
	// 模型构建
	// モデルの構築
	DrawMesh(DeltaTime);
}

void FRenderLayer::PostDraw(float DeltaTime)
{
	// 删除已被释放的RenderData弱指针
	// 解放済みのRenderData弱参照を削除する
	vector<vector<std::weak_ptr<FRenderingData>>::const_iterator> RemoveRenderingData;
	for (vector<std::weak_ptr<FRenderingData>>::const_iterator Iter = RenderDatas.begin();
		Iter != RenderDatas.end();
		++Iter)
	{
		if (Iter->expired())
		{
			RemoveRenderingData.push_back(Iter);
		}
	}

	for (auto& Tmp : RemoveRenderingData)
	{
		RenderDatas.erase(Tmp);
	}
}

void FRenderLayer::DrawObject(float DeltaTime, std::weak_ptr<FRenderingData>& InWeakRenderingData, ERenderingConditions RC)
{
	
	// 弱指针是不是被释放了
	// ポインタが解放されているかどうかを確認する
	if (InWeakRenderingData.expired()) 
	{
		return;
	}

	if (std::shared_ptr<FRenderingData> InRenderingData = InWeakRenderingData.lock())
	{
		auto GetRenderingConditions = [&]() -> bool
			{
				// 设置移动箭头是否可显示
				// 移動用の矢印を表示するかどうかを設定する
				if (InRenderingData->Mesh->IsVisible())
				{
					switch (RC)
					{
						case RC_Shadow:
						{
							return InRenderingData->Mesh->IsCastShadow();
						}
					}

					return true;
				}
				return false;
			};

		if (GetRenderingConditions())
		{
			CMaterial* Material = InRenderingData->Mesh->GetMaterialBySlot(InRenderingData->MaterialSlotIndex);
			if (!ShouldDrawInCurrentLayer(*InRenderingData, Material))
			{
				return;
			}

			UINT MeshOffset = GeometryMap->MeshConstantBufferViews.GetConstantBufferByteSize();

			D3D12_VERTEX_BUFFER_VIEW VBV = GeometryMap->Geometrys[InRenderingData->GeometryKey].GetVertexBufferView();
			D3D12_INDEX_BUFFER_VIEW IBV = GeometryMap->Geometrys[InRenderingData->GeometryKey].GetIndexBufferView();

			D3D12_GPU_VIRTUAL_ADDRESS FirstVirtualMeshAddress = GeometryMap->MeshConstantBufferViews.GetBuffer()->GetGPUVirtualAddress();
		
			GetGraphicsCommandList()->IASetIndexBuffer(&IBV);
			//	GetGraphicsCommandList()->OMSetBlendFactor();
			// 绑定渲染流水线上的输入槽，可以在输入装配器阶段传入顶点数据
			// レンダリングパイプラインの入力スロットをバインドし、入力アセンブラステージで頂点データを渡すことができる
			GetGraphicsCommandList()->IASetVertexBuffers(
				0,// 0-15 
				1,//k k+1 ... k+n-1 
				&VBV);

			// 定义要绘制的图元
			// 描画するプリミティブを定義する
			if (Material)
			{
				D3D_PRIMITIVE_TOPOLOGY DisplayStatus = Material->GetMaterialDisplayStatus();
				GetGraphicsCommandList()->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)DisplayStatus);
			}

			// 每个对象相对首地址的偏移
			// 各オブジェクトの先頭アドレスに対するオフセット
			D3D12_GPU_VIRTUAL_ADDRESS VAddress =
				FirstVirtualMeshAddress + InRenderingData->MeshObjectIndex * MeshOffset;

			GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(0, VAddress);

			// 绘制
			// 描画
			GetGraphicsCommandList()->DrawIndexedInstanced(  
				InRenderingData->IndexSize,//顶点数量         // 頂点数
				1,//绘制实例数量                              // 描画インスタンス数
				InRenderingData->IndexOffsetPosition,//顶点缓冲区第一个被绘制的索引                // 頂点バッファで最初に描画されるインデックス
				InRenderingData->VertexOffsetPosition,//GPU 从索引缓冲区读取的第一个索引的位置     // GPUがインデックスバッファから最初に読み取るインデックスの位置
				0);//在从顶点缓冲区读取每个实例数据之前添加到每个索引的值。                        // 頂点バッファから各インスタンスデータを読み取る前に各インデックスに加算する値
		}
	}
}

bool FRenderLayer::ShouldDrawInCurrentLayer(const FRenderingData& InRenderingData, const CMaterial* InMaterial) const
{
	const int CurrentLayerType = GetRenderLayerType();
	const int MeshLayerType = (int)InRenderingData.Mesh->GetRenderLayerType();

	if (InMaterial == nullptr)
	{
		return CurrentLayerType == MeshLayerType;
	}

	if (InMaterial->IsUseGlass())
	{
		return CurrentLayerType == (int)EMeshRenderLayerType::RENDERLAYER_TRANSPARENT;
	}

	return CurrentLayerType == MeshLayerType;
}

void FRenderLayer::FindObjectDraw(float DeltaTime, const CMeshComponent* InKey)
{
	for (auto& InRenderingData : RenderDatas)
	{
		// 判断指针是否被释放
		// ポインタが解放されているかどうかを判定する
		if (!InRenderingData.expired())
		{
			if (InRenderingData.lock()->Mesh == InKey)
			{
				DrawObject(DeltaTime, InRenderingData);
			}
		}
	}
}
void FRenderLayer::BuildPSO()
{
	// 先构建参数
	// まずパラメータを構築する
	DirectXPipelineState->BuildParam();

	BuildShader();

	// 绑定代理
	// プロキシをバインドする
	if (BuildPSODelegate.IsBound())
	{
		BuildPSODelegate.Execute(DirectXPipelineState->GetGPSDesc());
	}

}

void FRenderLayer::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	for (auto& InWeakRenderingData : RenderDatas)
	{
		if (!InWeakRenderingData.expired())
		{
			if (std::shared_ptr<FRenderingData> InRenderingData = InWeakRenderingData.lock())
			{
				//构造模型的world
				// モデルのワールド行列を構築する
				{
					XMFLOAT3& Position = InRenderingData->Mesh->GetPosition();
					fvector_3d Scale = InRenderingData->Mesh->GetScale();

					XMFLOAT3 RightVector = InRenderingData->Mesh->GetRightVector();
					XMFLOAT3 UPVector = InRenderingData->Mesh->GetUPVector();
					XMFLOAT3 ForwardVector = InRenderingData->Mesh->GetForwardVector();

					EngineMath::BuildMatrix(
						InRenderingData->WorldMatrix,
						Position,
						Scale,
						RightVector,
						UPVector,
						ForwardVector);
				}

				// 更新模型位置
				// モデルの位置を更新する
				XMMATRIX ATRIXWorld = XMLoadFloat4x4(&InRenderingData->WorldMatrix);
				XMMATRIX ATRIXTextureTransform = XMLoadFloat4x4(&InRenderingData->TextureTransform);

				// 法线矩阵
				// 法線行列
				XMVECTOR AATRIXWorldDeterminant = XMMatrixDeterminant(ATRIXWorld);
				XMMATRIX NormalInverseMatrix = XMMatrixInverse(&AATRIXWorldDeterminant, ATRIXWorld);

				FObjectTransform ObjectTransformation;
				XMStoreFloat4x4(&ObjectTransformation.World, XMMatrixTranspose(ATRIXWorld));
				XMStoreFloat4x4(&ObjectTransformation.TextureTransformation, XMMatrixTranspose(ATRIXTextureTransform));
				XMStoreFloat4x4(&ObjectTransformation.NormalTransformation, NormalInverseMatrix);

				//收集材质Index
				// マテリアルのインデックスを収集する
				if (CMaterial* InMater = InRenderingData->Mesh->GetMaterialBySlot(InRenderingData->MaterialSlotIndex))
				{
					ObjectTransformation.MaterialIndex = InMater->GetMaterialIndex();
				}

				GeometryMap->MeshConstantBufferViews.Update(InRenderingData->MeshObjectIndex, &ObjectTransformation);
			}
		}
	}
}

void FRenderLayer::ResetPSO()
{

}
void FRenderLayer::ResetPSO(EPipelineState InPipelineState)
{
}
void FRenderLayer::DrawWithPipelineState(float DeltaTime, EPipelineState InPipelineState)
{
	ResetPSO(InPipelineState);
	DrawMesh(DeltaTime);
}

void FRenderLayer::DrawMesh(float DeltaTime, ERenderingConditions RC)
{
	for (auto& InRenderingData : RenderDatas)
	{
		DrawObject(DeltaTime, InRenderingData, RC);
	}
}



std::wstring FRenderLayer::BuildShadersPaths(const std::wstring& InShadersHLSLName)
{
	return FEnginePathHelper::GetEngineShadersPath() + L"\\" + InShadersHLSLName + L".hlsl";
}

void FRenderLayer::Add(std::weak_ptr<FRenderingData> InRenderingData)
{
	RenderDatas.push_back(InRenderingData);
}

void FRenderLayer::Remove(std::weak_ptr<FRenderingData> InRenderingData)
{
	if (!InRenderingData.expired())
	{
		for (vector<std::weak_ptr<FRenderingData>>::const_iterator Iter = RenderDatas.begin();
			Iter != RenderDatas.end();
			++Iter)
		{
			if (!Iter->expired())
			{
				if (Iter->lock() == InRenderingData.lock())
				{
					RenderDatas.erase(Iter);
					break;
				}
			}
		}
	}
}

void FRenderLayer::Clear()
{
	RenderDatas.clear();
}
