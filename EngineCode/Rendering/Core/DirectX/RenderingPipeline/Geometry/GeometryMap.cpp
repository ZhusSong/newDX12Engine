#include "GeometryMap.h"
#include "../../../Buffer/ConstructBuffer.h"
#include "../../../../../Mesh/Core/ObjectTransform.h"
#include "../../../../../Core/Viewport/ViewportTransformation.h"
#include "../../../../../Mesh/Core/Mesh.h"
#include "../../../../../Mesh/Core/Material/MaterialConstantBuffer.h"
#include "../../../../../Component/Light/Core/LightConstantBuffer.h"
#include "../../../../../Component/Light/SpotLightComponent.h"
#include "../../../../../Mesh/Core/Material/Material.h"
#include "../../../../../Component/Mesh/Core/MeshComponent.h"
#include "../../../../../Manager/LightManager.h"
#include "../../../../../Component/Light/Core/LightComponent.h"
#include "../../../RenderingTextureResourcesUpdate.h"
#include "../RenderLayer/RenderLayerManager.h"

#include "../../../../../Component/Sky/FogComponent.h"
#include "../../../../../Component/Sky/SkyConstantBuffer.h"


UINT MeshObjectCount = 0;

FGeometryMap::FGeometryMap()
{
	Geometrys.insert(pair<int, FGeometry>(0, FGeometry()));

	RenderingTexture2DResources = std::make_shared<FRenderingTextureResourcesUpdate>();
	RenderingTexture2DResources->SetViewDimension(D3D12_SRV_DIMENSION_TEXTURE2D);

	RenderingCubeMapResources = std::make_shared<FRenderingTextureResourcesUpdate>();
	RenderingCubeMapResources->SetViewDimension(D3D12_SRV_DIMENSION_TEXTURECUBE);

	Fog = NULL;
}

FGeometryMap::~FGeometryMap()
{
	MeshObjectCount = 0;
}

void FGeometryMap::PreDraw(float DeltaTime)
{
	DescriptorHeap.PreDraw(DeltaTime);
}

void FGeometryMap::Draw(float DeltaTime)
{
	// 绘制灯光
	DrawLight(DeltaTime);

	// 绘制贴图
	Draw2DTexture(DeltaTime);

	//绘制立方体贴图
	DrawCubeMapTexture(DeltaTime);

	// 绘制材质
	DrawMaterial(DeltaTime);

	// 绘制雾
	DrawFog(DeltaTime);
}

void FGeometryMap::PostDraw(float DeltaTime)
{

}

void FGeometryMap::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	UpdateMaterialShaderResourceView(DeltaTime, ViewportInfo);

	//更新灯
	UpdateLight(DeltaTime, ViewportInfo);

	//更新视口
	UpdateCalculationsViewport(DeltaTime, ViewportInfo, 0);

	UpdateFog(DeltaTime, ViewportInfo);

	//更新视口
	DynamicShadowMap.UpdateCalculations(DeltaTime, ViewportInfo);

}

void FGeometryMap::UpdateCalculationsViewport(float DeltaTime, const FViewportInfo& ViewportInfo, UINT InConstantBufferOffset)
{
	XMMATRIX ViewMatrix = XMLoadFloat4x4(&ViewportInfo.ViewMatrix);
	XMMATRIX ProjectMatrix = XMLoadFloat4x4(&ViewportInfo.ProjectMatrix);
	XMMATRIX ViewProject = XMMatrixMultiply(ViewMatrix, ProjectMatrix);

	FViewportTransformation ViewportTransformation;
	XMStoreFloat4x4(&ViewportTransformation.ViewProjectionMatrix, XMMatrixTranspose(ViewProject));

	//拿到视口位置
	ViewportTransformation.ViewportPosition = ViewportInfo.ViewPosition;

	ViewportConstantBufferViews.Update(InConstantBufferOffset, &ViewportTransformation);
}


void FGeometryMap::UpdateMaterialShaderResourceView(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	FMaterialConstantBuffer MaterialConstantBuffer;
	for (size_t i = 0; i < Materials.size(); i++)
	{
		// 变换材质
		if (CMaterial* InMaterial = Materials[i])
		{
			if (InMaterial->IsDirty())
			{
				//BaseColor
				MaterialConstantBuffer.BaseColor = EngineMath::ToFloat4(InMaterial->GetBaseColor());

				//高光颜色
				MaterialConstantBuffer.SpecularColor = EngineMath::ToFloat3(InMaterial->GetSpecularColor());

				//粗糙度
				MaterialConstantBuffer.Roughness = InMaterial->GetRoughness();

				//折射率
				MaterialConstantBuffer.Refraction = InMaterial->GetRefractiveValue();

				//类型输入
				MaterialConstantBuffer.MaterialType = InMaterial->GetMaterialType();

				//F0输入
				fvector_3d F0 = InMaterial->GetFresnelF0();
				MaterialConstantBuffer.FresnelF0 = XMFLOAT3(F0.x, F0.y, F0.z);

				//透明度
				MaterialConstantBuffer.Transparency = InMaterial->GetTransparency();

				// 金属度
				MaterialConstantBuffer.Metallicity = EngineMath::ToFloat3(InMaterial->GetMetallicity());

				// 外部资源导入
				{
					// BaseColor
					if (auto BaseColorTextureResourcesPtr = FindRenderingTexture(InMaterial->GetBaseColorIndexKey()))
					{
						MaterialConstantBuffer.BaseColorIndex = (*BaseColorTextureResourcesPtr)->RenderingTextureID;
					}
					else
					{
						MaterialConstantBuffer.BaseColorIndex = -1;
					}


					// 法线
					if (auto NormalTextureResourcesPtr = FindRenderingTexture(InMaterial->GetNormalIndexKey()))
					{
						MaterialConstantBuffer.NormalIndex = (*NormalTextureResourcesPtr)->RenderingTextureID;
					}
					else
					{
						MaterialConstantBuffer.NormalIndex = -1;
					}

					// 高光
					if (auto SpecularTextureResourcesPtr = FindRenderingTexture(InMaterial->GetSpecularKey()))
					{
						MaterialConstantBuffer.SpecularIndex = (*SpecularTextureResourcesPtr)->RenderingTextureID;
					}
					else
					{
						MaterialConstantBuffer.SpecularIndex = -1;
					}
				}

				//材质矩阵
				XMMATRIX MaterialTransform = XMLoadFloat4x4(&InMaterial->GetMaterialTransform());
				XMStoreFloat4x4(&MaterialConstantBuffer.TransformInformation,
					XMMatrixTranspose(MaterialTransform));

				InMaterial->SetDirty(false);

				MaterialConstantBufferViews.Update(InMaterial->GetMaterialIndex(), &MaterialConstantBuffer);
			}
		}
	}
}

void FGeometryMap::UpdateLight(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	//更新灯光
	FLightConstantBuffer LightConstantBuffer;
	for (size_t i = 0; i < GetLightManager()->Lights.size(); i++)
	{
		if (CLightComponent* InLightComponent = GetLightManager()->Lights[i])
		{
			fvector_3d LightIntensity = InLightComponent->GetLightIntensity();
			LightConstantBuffer.SceneLights[i].LightIntensity = XMFLOAT3(LightIntensity.x, LightIntensity.y, LightIntensity.z);
			LightConstantBuffer.SceneLights[i].LightDirection = InLightComponent->GetForwardVector();

			LightConstantBuffer.SceneLights[i].Position = InLightComponent->GetPosition();
			LightConstantBuffer.SceneLights[i].LightType = InLightComponent->GetLightType();

			switch (InLightComponent->GetLightType())
			{
			case ELightType::DirectionalLight:
			{
				// 正交矩阵
				XMFLOAT3 ForwardVector = InLightComponent->GetForwardVector();

				DynamicShadowMap.BuildParallelLightMatrix(
					EngineMath::ToVector3d(ForwardVector), fvector_3d(0.f), 70.f);

				XMFLOAT4X4 ShadowViewMatrix;
				XMFLOAT4X4 ShadowProjectMatrix;
				DynamicShadowMap.GetViewportMatrix(ShadowViewMatrix, ShadowProjectMatrix);

				XMMATRIX ShadowViewMatrixRTX = XMLoadFloat4x4(&ShadowViewMatrix);
				XMMATRIX ShadowProjectMatrixRTX = XMLoadFloat4x4(&ShadowProjectMatrix);

				// NDC [-1,1]; = >[0,1]
				// 半兰伯特
				XMMATRIX Transform =
				{
					0.5f, 0.0f, 0.0f, 0.0f,
					0.0f, -0.5f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.5f, 0.5f, 0.0f, 1.0f
				};

				XMMATRIX ShadowViewProjectMatrixRTX =
					ShadowViewMatrixRTX * ShadowProjectMatrixRTX * Transform;

				//存储Shadow变换信息
				XMStoreFloat4x4(&LightConstantBuffer.SceneLights[i].ShadowTransform, XMMatrixTranspose(ShadowViewProjectMatrixRTX));

				break;
			}
			case ELightType::PointLight:
			case ELightType::SpotLight:
			{
				if (CRangeLightComponent* InRangeLightComponent = dynamic_cast<CRangeLightComponent*>(InLightComponent))
				{
					LightConstantBuffer.SceneLights[i].StartAttenuation = InRangeLightComponent->GetStartAttenuation();
					LightConstantBuffer.SceneLights[i].EndAttenuation = InRangeLightComponent->GetEndAttenuation();
				}

				if (InLightComponent->GetLightType() == ELightType::SpotLight)
				{
					if (CSpotLightComponent* InSpotLightComponent = dynamic_cast<CSpotLightComponent*>(InLightComponent))
					{
						LightConstantBuffer.SceneLights[i].ConicalInnerCorner = math_utils::angle_to_radian(InSpotLightComponent->GetConicalInnerCorner());
						LightConstantBuffer.SceneLights[i].ConicalOuterCorner = math_utils::angle_to_radian(InSpotLightComponent->GetConicalOuterCorner());
					}
				}

				break;
			}
			}
		}
	}

	LightConstantBufferViews.Update(0, &LightConstantBuffer);
}
void FGeometryMap::UpdateFog(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	//更新雾
	if (Fog)
	{
		if (Fog->IsDirty())
		{
			FFogConstantBuffer FogConstantBuffer;
			{
				fvector_color FogColor = Fog->GetFogColor();
				FogConstantBuffer.FogColor = XMFLOAT3(FogColor.r, FogColor.g, FogColor.b);

				FogConstantBuffer.FogStart = Fog->GetFogStart();
				FogConstantBuffer.FogRange = Fog->GetFogRange();
				FogConstantBuffer.FogHeight = Fog->GetFogHeight();
				FogConstantBuffer.FogTransparentCoefficient = Fog->GetFogTransparentCoefficient();
			}

			FogConstantBufferViews.Update(0, &FogConstantBuffer);

			Fog->SetDirty(false);
		}
	}
}


void FGeometryMap::BuildFog()
{
	for (auto& Tmp : GObjects)
	{
		if (CFogComponent* InFogComponent = dynamic_cast<CFogComponent*>(Tmp))
		{
			Fog = InFogComponent;
			break;
		}
	}
}
void FGeometryMap::BuildShadow()
{
	DynamicShadowMap.Init(2048, 2048);

	DynamicShadowMap.BuildViewport(fvector_3d(0.f, 0.f, 0.f));

	DynamicShadowMap.BuildDepthStencilDescriptor();

	DynamicShadowMap.BuildRenderTargetDescriptor();
}
void FGeometryMap::BuildDynamicReflectionMesh()
{
	for (auto& Tmp : GObjects)
	{
		if (CMeshComponent* InMeshComponent = dynamic_cast<CMeshComponent*>(Tmp))
		{
			if (InMeshComponent->IsDynamicReflection())
			{
				DynamicReflectionMeshComponents.push_back(InMeshComponent);
			}
		}
	}
}

void FGeometryMap::BuildMesh(const size_t InMeshHash, CMeshComponent* InMesh, const FMeshRenderingData& MeshData)
{
	for (auto& Tmp : Geometrys)
	{
		Tmp.second.BuildMesh(InMeshHash, InMesh, MeshData, Tmp.first);
	}
}

void FGeometryMap::DuplicateMesh(CMeshComponent* InMesh, const FRenderingData& MeshData)
{
	for (auto& Tmp : Geometrys)
	{
		Tmp.second.DuplicateMesh(InMesh, MeshData, Tmp.first);
	}
}

bool FGeometryMap::FindMeshRenderingDataByHash(const size_t& InHash, FRenderingData& MeshData, int InRenderLayerIndex)
{
	for (auto& Tmp : Geometrys)
	{
		if (Tmp.second.FindMeshRenderingDataByHash(InHash, MeshData, InRenderLayerIndex))
		{
			return true;
		}
	}


	return false;
}
void FGeometryMap::LoadTexture()
{
	def_c_paths Paths;
	init_def_c_paths(&Paths);

	char RootPath[] = "../newDX12Engine/Asset";
	find_files(RootPath, &Paths, true);

	for (int i = 0; i < Paths.index; i++)
	{
		if (find_string(Paths.paths[i], ".dds", 0) != -1)
		{
			//单位化路径
			normalization_path(Paths.paths[i]);

			wchar_t TexturePath[1024] = { 0 };
			char_to_wchar_t(TexturePath, 1024, Paths.paths[i]);

			if (wfind_string(TexturePath, L"_CubeMap.") != -1 ||
				wfind_string(TexturePath, L"_cubemap.") != -1)
			{
				//CubeMap
				RenderingCubeMapResources->LoadTextureResources(TexturePath);
			}
			else
			{
				//Texture2D
				RenderingTexture2DResources->LoadTextureResources(TexturePath);
			}
		}
	}
}

void FGeometryMap::Build()
{
	//构建模型
	for (auto& Tmp : Geometrys)
	{
		Tmp.second.Build();
	}
}

void FGeometryMap::BuildDescriptorHeap()
{
	// 只构建贴图描述符表
	DescriptorHeap.Build(
		GetDrawTexture2DResourcesNumber() + //Texture2D
		GetDrawCubeMapResourcesNumber() + //静态Cube贴图
		1+		//动态Cube贴图
		1);		//阴影
}

void FGeometryMap::BuildMeshConstantBuffer()
{
	//创建常量缓冲区
	MeshConstantBufferViews.CreateConstant(sizeof(FObjectTransform), GetDrawMeshObjectNumber());

}

void FGeometryMap::BuildFogConstantBuffer()
{
	// 创建雾常量缓冲区视图
	FogConstantBufferViews.CreateConstant(sizeof(FFogConstantBuffer), 1);
}

void FGeometryMap::BuildMaterialShaderResourceView()
{
	// 收集材质
	// 更新Shader-Index
	for (auto& Tmp : FRenderLayerManager::RenderLayers)
	{
		for (auto& InData : Tmp->RenderDatas)
		{
			if (auto InMaterials = InData.Mesh->GetMaterials())
			{
				for (size_t j = 0; j < InMaterials->size(); j++)
				{
					(*InMaterials)[j]->SetMaterialIndex(Materials.size());

					Materials.push_back((*InMaterials)[j]);
				}
			}
		}
	}

	// 创建常量缓冲区
	MaterialConstantBufferViews.CreateConstant(
		sizeof(FMaterialConstantBuffer),
		GetDrawMaterialObjectNumber(),
		false);
}

void FGeometryMap::BuildLightConstantBuffer()
{
	// 创建常量缓冲区
	LightConstantBufferViews.CreateConstant(sizeof(FLightConstantBuffer), GetDrawLightObjectNumber());

}

UINT FGeometryMap::GetDrawMeshObjectNumber()
{
	return Geometrys[0].GetDrawObjectNumber();
}

UINT FGeometryMap::GetDrawMaterialObjectNumber()
{
	return Materials.size();
}

UINT FGeometryMap::GetDrawLightObjectNumber()
{
	return 1;
}

UINT FGeometryMap::GetDrawTexture2DResourcesNumber()
{
	return RenderingTexture2DResources->Size();
}

UINT FGeometryMap::GetDrawCubeMapResourcesNumber()
{
	return RenderingCubeMapResources->Size();
}

UINT FGeometryMap::GetDynamicReflectionViewportNum()
{
	return DynamicReflectionMeshComponents.size() * 6;
}
void FGeometryMap::BuildTextureConstantBuffer()
{
	// 构建2D贴图
	RenderingTexture2DResources->BuildTextureConstantBuffer(
		DescriptorHeap.GetHeap(), 0);//视口

	// 构建CubeMap贴图
	RenderingCubeMapResources->BuildTextureConstantBuffer(
		DescriptorHeap.GetHeap(),
		GetDrawTexture2DResourcesNumber());//加上2D贴图地址偏移
}

void FGeometryMap::BuildViewportConstantBufferView(UINT InViewportOffset)
{
	//创建常量缓冲区
	ViewportConstantBufferViews.CreateConstant(sizeof(FViewportTransformation),
		1 + //主视口 摄像机
		GetDynamicReflectionViewportNum() + //这个是动态反射的视口
		1 + //阴影视口
		InViewportOffset);


}

bool FGeometryMap::IsStartUPFog()
{
	return Fog != NULL;
}
std::unique_ptr<FRenderingTexture>* FGeometryMap::FindRenderingTexture(const std::string& InKey)
{
	if (auto RenderingTexture2DPtr = RenderingTexture2DResources->FindRenderingTexture(InKey))
	{
		return RenderingTexture2DPtr;
	}
	else if (auto RenderingCubeMapPtr = RenderingCubeMapResources->FindRenderingTexture(InKey))
	{
		return RenderingCubeMapPtr;
	}

	return nullptr;
}

void FGeometryMap::DrawShadow(float DeltaTime)
{
	DynamicShadowMap.Draw(DeltaTime);
}

void FGeometryMap::DrawLight(float DeltaTime)
{

	GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(
		2,
		LightConstantBufferViews.GetBuffer()->GetGPUVirtualAddress());
}

void FGeometryMap::DrawViewport(float DeltaTime)
{

	GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(
		1,
		ViewportConstantBufferViews.GetBuffer()->GetGPUVirtualAddress());
}

void FGeometryMap::DrawMesh(float DeltaTime)
{

}

void FGeometryMap::DrawMaterial(float DeltaTime)
{
	GetGraphicsCommandList()->SetGraphicsRootShaderResourceView(
		4,
		MaterialConstantBufferViews.GetBuffer()->GetGPUVirtualAddress());
}

void FGeometryMap::Draw2DTexture(float DeltaTime)
{
	UINT DescriptorOffset = GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();

	auto DesHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GetHeap()->GetGPUDescriptorHandleForHeapStart());
	DesHandle.Offset(0, DescriptorOffset);

	GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(5, DesHandle);
}

void FGeometryMap::DrawCubeMapTexture(float DeltaTime)
{
	UINT DescriptorOffset = GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();

	auto DesHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GetHeap()->GetGPUDescriptorHandleForHeapStart());
	DesHandle.Offset(GetDrawTexture2DResourcesNumber(), DescriptorOffset);

	GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(6, DesHandle);
}

void FGeometryMap::DrawFog(float DeltaTime)
{
	GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(
		3,
		FogConstantBufferViews.GetBuffer()->GetGPUVirtualAddress());
}

void FGeometry::BuildMesh(
	const size_t InMeshHash,
	CMeshComponent* InMesh,
	const FMeshRenderingData& MeshData,
	int InKey)
{
	if (!IsRenderingDataExistence(InMesh))
	{
		// 找到对应层级
		if (std::shared_ptr<FRenderLayer> InRenderLayer = FRenderLayerManager::FindByRenderLayer((int)InMesh->GetRenderLayerType()))
		{
			InRenderLayer->RenderDatas.push_back(FRenderingData());
			FRenderingData& InRenderingData = InRenderLayer->RenderDatas[InRenderLayer->RenderDatas.size() - 1];

			// 基础注册
			InRenderingData.MeshObjectIndex = MeshObjectCount++;
			InRenderingData.Mesh = InMesh;
			InRenderingData.MeshHash = InMeshHash;
			InRenderingData.GeometryKey = InKey;

			InRenderingData.IndexSize = MeshData.IndexData.size();
			InRenderingData.VertexSize = MeshData.VertexData.size();

			InRenderingData.IndexOffsetPosition = MeshRenderingData.IndexData.size();
			InRenderingData.VertexOffsetPosition = MeshRenderingData.VertexData.size();

			// 插入
			// 合并索引
			MeshRenderingData.IndexData.insert(
				MeshRenderingData.IndexData.end(),
				MeshData.IndexData.begin(),
				MeshData.IndexData.end());

			// 合并顶点
			MeshRenderingData.VertexData.insert(
				MeshRenderingData.VertexData.end(),
				MeshData.VertexData.begin(),
				MeshData.VertexData.end());
		}
	}
}

void FGeometry::DuplicateMesh(CMeshComponent* InMesh, const FRenderingData& MeshData, int InKey)
{
	if (!IsRenderingDataExistence(InMesh))
	{
		if (std::shared_ptr<FRenderLayer> InRenderLayer = FRenderLayerManager::FindByRenderLayer((int)InMesh->GetRenderLayerType()))
		{
			InRenderLayer->RenderDatas.push_back(MeshData);
			FRenderingData& InRenderingData = InRenderLayer->RenderDatas[InRenderLayer->RenderDatas.size() - 1];

			// 基础注册
			InRenderingData.Mesh = InMesh;
			InRenderingData.MeshObjectIndex = MeshObjectCount++;
			InRenderingData.GeometryKey = InKey;
		}
	}
}

bool FGeometry::IsRenderingDataExistence(CMeshComponent* InKey)
{
	if (std::shared_ptr<FRenderLayer> InRenderLayer = FRenderLayerManager::FindByRenderLayer((int)InKey->GetRenderLayerType()))
	{
		for (auto& Tmp : InRenderLayer->RenderDatas)
		{
			if (Tmp.Mesh == InKey)
			{
				return true;
			}
		}
	}

	return false;
}

bool FGeometry::FindMeshRenderingDataByHash(const size_t& InHash, FRenderingData& MeshData, int InRenderLayerIndex)
{
	//寻找RenderData
	auto FindMeshRenderingDataByHashSub = [&](std::shared_ptr<FRenderLayer> InRenderLayer)->FRenderingData*
		{
			for (auto& SubTmp : InRenderLayer->RenderDatas)
			{
				if (SubTmp.MeshHash == InHash)
				{
					return &SubTmp;
				}
			}

			return NULL;
		};

	if (InRenderLayerIndex == -1)//开启暴力遍历
	{
		for (auto& Tmp : FRenderLayerManager::RenderLayers)
		{
			if (FRenderingData* InRenderingData = FindMeshRenderingDataByHashSub(Tmp))
			{
				MeshData = *InRenderingData;
				return true;
			}
		}
	}
	//精准寻找
	else if (std::shared_ptr<FRenderLayer> InRenderLayer = FRenderLayerManager::FindByRenderLayer(InRenderLayerIndex))
	{
		if (FRenderingData* InRenderingData = FindMeshRenderingDataByHashSub(InRenderLayer))
		{
			MeshData = *InRenderingData;
			return true;
		}
	}

	return false;
}

UINT FGeometry::GetDrawObjectNumber() const
{
	return MeshObjectCount;

	int Count = 0;
	for (auto& Tmp : FRenderLayerManager::RenderLayers)
	{
		for (auto& SubTmp : Tmp->RenderDatas)
		{
			Count++;
		}
	}

	return Count;
}

void FGeometry::Build()
{
	UINT VertexSizeInBytes = MeshRenderingData.GetVertexSizeInBytes();
	UINT IndexSizeInBytes = MeshRenderingData.GetIndexSizeInBytes();

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

D3D12_VERTEX_BUFFER_VIEW FGeometry::GetVertexBufferView()
{
	D3D12_VERTEX_BUFFER_VIEW VBV;
	VBV.BufferLocation = GPUVertexBufferPtr->GetGPUVirtualAddress();
	VBV.SizeInBytes = MeshRenderingData.GetVertexSizeInBytes();
	VBV.StrideInBytes = sizeof(FVertex);

	return VBV;
}

D3D12_INDEX_BUFFER_VIEW FGeometry::GetIndexBufferView()
{
	D3D12_INDEX_BUFFER_VIEW IBV;
	IBV.BufferLocation = GPUIndexBufferPtr->GetGPUVirtualAddress();
	IBV.SizeInBytes = MeshRenderingData.GetIndexSizeInBytes();
	IBV.Format = DXGI_FORMAT_R16_UINT;

	return IBV;
}

