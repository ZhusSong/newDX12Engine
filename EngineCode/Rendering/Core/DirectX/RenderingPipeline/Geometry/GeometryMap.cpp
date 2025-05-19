﻿#include "GeometryMap.h"
#include "../../../Buffer/ConstructBuffer.h"
#include "../../../../../Mesh/Core/ObjectTransform.h"
#include "../../../../../Core/Viewport/ViewportTransformation.h"
#include "../../../../../Mesh/Core/Mesh.h"
#include "../../../../../Mesh/Core/Material/MaterialConstantBuffer.h"
#include "../../../../../Component/Light/Core/LightConstantBuffer.h"
#include "../../../../../Mesh/Core/Material/Material.h"
#include "../../../../../Component/Mesh/Core/MeshComponent.h"

#include "../../../../../Manager/LightManager.h"
#include "../../../../../Component/Light/Core/LightComponent.h"

bool FGeometry::bRenderingDataExistence(CMeshComponent* InKey)
{
	for (auto& Tmp : DescribeMeshRenderingData)
	{
		if (Tmp.Mesh == InKey)
		{
			return true;
		}
	}

	return false;
}
void FGeometry::BuildMesh(CMeshComponent* InMesh, const FMeshRenderingData& MeshData)
{
	// 判断当前模型数据是否已添加过
	if (!bRenderingDataExistence(InMesh))
	{
		DescribeMeshRenderingData.push_back(FRenderingData());

		// 获取刚刚添加的数据
		FRenderingData& InRenderingData = DescribeMeshRenderingData[DescribeMeshRenderingData.size() - 1];

		InRenderingData.Mesh = InMesh;

		// 记录数据尺寸
		InRenderingData.IndexSize = MeshData.IndexData.size();
		InRenderingData.VertexSize = MeshData.VertexData.size();

		// 记录偏移位置
		InRenderingData.IndexOffsetPosition = MeshRenderingData.IndexData.size();
		InRenderingData.VertexOffsetPosition = MeshRenderingData.VertexData.size();


		// 插入渲染数据
		// 添加索引数据
		MeshRenderingData.IndexData.insert(
			MeshRenderingData.IndexData.end(), // 位置为当前列表尾部
			MeshData.IndexData.begin(),        // 本次数据头部
			MeshData.IndexData.end());         // 本次数据尾部

		// 添加顶点数据
		MeshRenderingData.VertexData.insert(
			MeshRenderingData.VertexData.end(),
			MeshData.VertexData.begin(),
			MeshData.VertexData.end());
	}
}

void FGeometry::Build()
{
	// 获取顶点与索引size
	UINT VertexSizeInBytes = MeshRenderingData.GetVertexSizeInBytes();
	UINT IndexSizeInBytes = MeshRenderingData.GetIndexSizeInBytes();

	// 拷贝到CPU顶点与索引buffer
	ANALYSIS_HRESULT(D3DCreateBlob(
		VertexSizeInBytes,
		&CPUVertexBufferPtr));

	memcpy(CPUVertexBufferPtr->GetBufferPointer(),
		MeshRenderingData.VertexData.data(), VertexSizeInBytes);

	ANALYSIS_HRESULT(D3DCreateBlob(
		IndexSizeInBytes,
		&CPUIndexBufferPtr));

	memcpy(CPUIndexBufferPtr->GetBufferPointer(),
		MeshRenderingData.IndexData.data(), IndexSizeInBytes);

	ConstructBuffer::FConstructBuffer ConstructBuffer;
	GPUVertexBufferPtr = ConstructBuffer.ConstructDefaultBuffer(
		VertexBufferTmpPtr,
		MeshRenderingData.VertexData.data(), VertexSizeInBytes);

	GPUIndexBufferPtr = ConstructBuffer.ConstructDefaultBuffer(
		IndexBufferTmpPtr,
		MeshRenderingData.IndexData.data(), IndexSizeInBytes);
}

// 返回顶点缓冲区视图
D3D12_VERTEX_BUFFER_VIEW FGeometry::GetVertexBufferView()
{
	D3D12_VERTEX_BUFFER_VIEW VBV;
	VBV.BufferLocation = GPUVertexBufferPtr->GetGPUVirtualAddress();
	VBV.SizeInBytes = MeshRenderingData.GetVertexSizeInBytes();
	VBV.StrideInBytes = sizeof(FVertex);

	return VBV;
}

// 返回索引缓冲区视图
D3D12_INDEX_BUFFER_VIEW FGeometry::GetIndexBufferView()
{
	D3D12_INDEX_BUFFER_VIEW IBV;
	IBV.BufferLocation = GPUIndexBufferPtr->GetGPUVirtualAddress();
	IBV.SizeInBytes = MeshRenderingData.GetIndexSizeInBytes();
	IBV.Format = DXGI_FORMAT_R16_UINT;

	return IBV;
}

// 初始化几何体数据
FGeometryMap::FGeometryMap()
{
	Geometrys.insert(pair<int, FGeometry>(0, FGeometry()));
}


void FGeometryMap::PreDraw(float DeltaTime)
{
	DescriptorHeap.PreDraw(DeltaTime);
}

void FGeometryMap::Draw(float DeltaTime)
{
	// 渲染视口
	DrawViewport(DeltaTime);

	// 绘制灯光
	DrawLight(DeltaTime);

	// 渲染模型
	DrawMesh(DeltaTime);
}

void FGeometryMap::PostDraw(float DeltaTime)
{

}

void FGeometryMap::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	XMMATRIX ViewMatrix = XMLoadFloat4x4(&ViewportInfo.ViewMatrix);
	XMMATRIX ProjectMatrix = XMLoadFloat4x4(&ViewportInfo.ProjectMatrix);

	for (auto& Tmp : Geometrys)
	{
		for (size_t i = 0; i < Tmp.second.DescribeMeshRenderingData.size(); i++)
		{
			FRenderingData& InRenderingData = Tmp.second.DescribeMeshRenderingData[i];

			//构造模型的world坐标
			{
				XMFLOAT3& Position = InRenderingData.Mesh->GetPosition();
				fvector_3d Scale = InRenderingData.Mesh->GetScale();

				XMFLOAT3 RightVector = InRenderingData.Mesh->GetRightVector();
				XMFLOAT3 UPVector = InRenderingData.Mesh->GetUPVector();
				XMFLOAT3 ForwardVector = InRenderingData.Mesh->GetForwardVector();

				InRenderingData.WorldMatrix = {
					RightVector.x * Scale.x,	UPVector.x,				ForwardVector.x ,			0.f,
					RightVector.y,				UPVector.y * Scale.x,	ForwardVector.y,			0.f,
					RightVector.z,				UPVector.z ,			ForwardVector.z * Scale.x,	0.f,
					Position.x,					Position.y,				Position.z,					1.f };
			}

			// 更新模型位置
			XMMATRIX ATRIXWorld = XMLoadFloat4x4(&InRenderingData.WorldMatrix);

			FObjectTransform ObjectTransformation;
			XMStoreFloat4x4(&ObjectTransformation.World, XMMatrixTranspose(ATRIXWorld));
			MeshConstantBufferViews.Update(i, &ObjectTransformation);
			
			//变换材质
			FMaterialConstantBuffer MaterialConstantBuffer;
			{
				if (CMaterial* InMaterial = (*InRenderingData.Mesh->GetMaterials())[0])
				{
					//BaseColor
					fvector_4d InBaseColor = InMaterial->GetBaseColor();
					MaterialConstantBuffer.BaseColor = XMFLOAT4(InBaseColor.x, InBaseColor.y, InBaseColor.z, InBaseColor.w);

					//粗糙度
					MaterialConstantBuffer.Roughness = InMaterial->GetRoughness();

					//类型输入
					MaterialConstantBuffer.MaterialType = InMaterial->GetMaterialType();
				}
			}
			MaterialConstantBufferViews.Update(i, &MaterialConstantBuffer);
		
		}
	}

	//更新灯光
	for (size_t i = 0; i < GetLightManager()->Lights.size(); i++)
	{
		FLightConstantBuffer LightConstantBuffer;
		{
			if (CLightComponent* InLightComponent = GetLightManager()->Lights[0])
			{
				LightConstantBuffer.LightDirection = InLightComponent->GetForwardVector();
			}
		}

		LightConstantBufferViews.Update(i, &LightConstantBuffer);
	}


	// 更新视口
	XMMATRIX ViewProject = XMMatrixMultiply(ViewMatrix, ProjectMatrix);
	FViewportTransformation ViewportTransformation;
	XMStoreFloat4x4(&ViewportTransformation.ViewProjectionMatrix, XMMatrixTranspose(ViewProject));
	
	//拿到视口位置
	ViewportTransformation.ViewportPosition = ViewportInfo.ViewPosition;

	ViewportConstantBufferViews.Update(0, &ViewportTransformation);
}

void FGeometryMap::BuildMesh(CMeshComponent* InMesh, const FMeshRenderingData& MeshData)
{
	FGeometry& Geometry = Geometrys[0];

	Geometry.BuildMesh(InMesh, MeshData);
}

void FGeometryMap::Build()
{
	// 构建模型
	for (auto& Tmp : Geometrys)
	{
		Tmp.second.Build();
	}
}

void FGeometryMap::BuildDescriptorHeap()
{
	//+1摄像机
	DescriptorHeap.Build(GetDrawMeshObjectNumber() + GetDrawMaterialObjectNumber() + 1 + GetDrawLightObjectNumber());
}


void FGeometryMap::BuildMeshConstantBuffer()
{
	// 创建常量缓冲区
	MeshConstantBufferViews.CreateConstant(sizeof(FObjectTransform), GetDrawMeshObjectNumber());

	//Handle
	CD3DX12_CPU_DESCRIPTOR_HANDLE DesHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(GetHeap()->GetCPUDescriptorHandleForHeapStart());

	// 构建常量缓冲区
	MeshConstantBufferViews.BuildConstantBuffer(DesHandle, GetDrawMeshObjectNumber());
}

void FGeometryMap::BuildMaterialConstantBuffer()
{
	// 创建常量缓冲区
	MaterialConstantBufferViews.CreateConstant(sizeof(FMaterialConstantBuffer), GetDrawMaterialObjectNumber());

	// Handle
	CD3DX12_CPU_DESCRIPTOR_HANDLE DesHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(GetHeap()->GetCPUDescriptorHandleForHeapStart());

	// 构建常量缓冲区
	MaterialConstantBufferViews.BuildConstantBuffer(
		DesHandle,
		GetDrawMaterialObjectNumber(),
		GetDrawMeshObjectNumber());
}

void FGeometryMap::BuildLightConstantBuffer()
{
	// 创建常量缓冲区
	LightConstantBufferViews.CreateConstant(sizeof(FLightConstantBuffer), GetDrawLightObjectNumber());

	// Handle
	CD3DX12_CPU_DESCRIPTOR_HANDLE DesHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(GetHeap()->GetCPUDescriptorHandleForHeapStart());

	// 构建常量缓冲区
	LightConstantBufferViews.BuildConstantBuffer(
		DesHandle,
		GetDrawLightObjectNumber(),
		GetDrawMeshObjectNumber() 
		+ GetDrawMaterialObjectNumber());
}

UINT FGeometryMap::GetDrawMeshObjectNumber()
{
	return Geometrys[0].GetDrawObjectNumber();
}

UINT FGeometryMap::GetDrawMaterialObjectNumber()
{
	UINT MaterialNum = 0;
	for (auto& Tmp : Geometrys)
	{
		for (auto& TmpSun : Tmp.second.DescribeMeshRenderingData)
		{
			MaterialNum += TmpSun.Mesh->GetMaterialNum();
		}
	}

	return MaterialNum;
}
void FGeometryMap::BuildViewportConstantBufferView()
{
	// 创建常量缓冲区
	ViewportConstantBufferViews.CreateConstant(sizeof(FViewportTransformation), 1);

	// Handle
	CD3DX12_CPU_DESCRIPTOR_HANDLE DesHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(DescriptorHeap.GetHeap()->GetCPUDescriptorHandleForHeapStart());

	// 构建常量缓冲区
	ViewportConstantBufferViews.BuildConstantBuffer(DesHandle,
		1,
		GetDrawMeshObjectNumber() +
		GetDrawMaterialObjectNumber() +
		GetDrawLightObjectNumber());
}


UINT FGeometryMap::GetDrawLightObjectNumber()
{
	return 1;
}

void FGeometryMap::DrawLight(float DeltaTime)
{
	UINT DescriptorOffset = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto DesHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GetHeap()->GetGPUDescriptorHandleForHeapStart());
	DesHandle.Offset(
		GetDrawMeshObjectNumber()
		+ GetDrawMaterialObjectNumber(), DescriptorOffset);

	GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(3, DesHandle);
}
void FGeometryMap::DrawViewport(float DeltaTime)
{
	UINT DescriptorOffset = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto DesHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GetHeap()->GetGPUDescriptorHandleForHeapStart());
	DesHandle.Offset(
		GetDrawMeshObjectNumber()
		+ GetDrawMaterialObjectNumber() +
		GetDrawLightObjectNumber(), DescriptorOffset);

	GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(1, DesHandle);
}
void FGeometryMap::DrawMesh(float DeltaTime)
{
	UINT DescriptorOffset = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 模型构建
	for (auto& Tmp : Geometrys)
	{
		D3D12_VERTEX_BUFFER_VIEW VBV = Tmp.second.GetVertexBufferView();
		D3D12_INDEX_BUFFER_VIEW IBV = Tmp.second.GetIndexBufferView();

		for (int i = 0; i < Tmp.second.DescribeMeshRenderingData.size(); i++)
		{
			auto DesMeshHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GetHeap()->GetGPUDescriptorHandleForHeapStart());
			auto DesMaterialHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GetHeap()->GetGPUDescriptorHandleForHeapStart());
			
			FRenderingData& InRenderingData = Tmp.second.DescribeMeshRenderingData[i];

			GetGraphicsCommandList()->IASetIndexBuffer(&IBV);

			// 绑定渲染流水线上的输入槽，可以在输入装配器阶段传入顶点数据
			GetGraphicsCommandList()->IASetVertexBuffers(
				0,//起始输入槽 0-15 
				1,//k k+1 ... k+n-1 
				&VBV);


			// 定义我们要绘制的哪种图元 点 线 面
			EMaterialDisplayStatusType DisplayStatus = (*InRenderingData.Mesh->GetMaterials())[0]->GetMaterialDisplayStatus();
			GetGraphicsCommandList()->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)DisplayStatus);

			// 模型起始地址偏移
			DesMeshHandle.Offset(i, DescriptorOffset);
			GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(0, DesMeshHandle);

			// 材质起始地址偏移
			DesMaterialHandle.Offset(i + GetDrawMeshObjectNumber(), DescriptorOffset);
			GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(2, DesMaterialHandle);

			// 真正的绘制
			GetGraphicsCommandList()->DrawIndexedInstanced(
				InRenderingData.IndexSize,//顶点数量
				1,//绘制实例数量
				InRenderingData.IndexOffsetPosition,//顶点缓冲区第一个被绘制的索引
				InRenderingData.VertexOffsetPosition,//GPU 从索引缓冲区读取的第一个索引的位置。
				0);//在从顶点缓冲区读取每个实例数据之前添加到每个索引的值。
		}
	}
}




