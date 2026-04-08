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

#include <unordered_set>


UINT MeshObjectCount = 0;

map<size_t, std::vector<FRenderingData>> FGeometry::UniqueRenderingDatas;
vector<std::shared_ptr<FRenderingData>> FGeometry::RenderingDatas;

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
	// ライトを描画
	DrawLight(DeltaTime);

	// 绘制贴图
	// テクスチャを描画
	Draw2DTexture(DeltaTime);

	//绘制立方体贴图
	// キューブマップテクスチャを描画
	DrawCubeMapTexture(DeltaTime);

	// 绘制材质
	// マテリアルを描画
	DrawMaterial(DeltaTime);

	// 绘制雾
	// フォグを描画
	DrawFog(DeltaTime);
}

void FGeometryMap::PostDraw(float DeltaTime)
{

}

void FGeometryMap::OnResetSize(int InWidth, int InHeight)
{
	DynamicShadowCubeMap.OnResetSize(InWidth, InHeight);
}

void FGeometryMap::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	UpdateMaterialShaderResourceView(DeltaTime, ViewportInfo);

	// 更新灯
	// ライトを更新
	UpdateLight(DeltaTime, ViewportInfo);

	// 更新视口
	// ビューポートを更新
	UpdateCalculationsViewport(DeltaTime, ViewportInfo, 0);

	// 更新雾
	// フォグを更新
	UpdateFog(DeltaTime, ViewportInfo);

	// 更新视口
	// ビューポートを更新
	DynamicShadowMap.UpdateCalculations(DeltaTime, ViewportInfo);

}

void FGeometryMap::UpdateCalculationsViewport(float DeltaTime, const FViewportInfo& ViewportInfo, UINT InConstantBufferOffset)
{
	XMMATRIX ViewMatrix = XMLoadFloat4x4(&ViewportInfo.ViewMatrix);
	XMMATRIX ProjectMatrix = XMLoadFloat4x4(&ViewportInfo.ProjectMatrix);
	XMMATRIX ViewProject = XMMatrixMultiply(ViewMatrix, ProjectMatrix);


	XMMATRIX HalfLambert(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX TexViewProjectionMatrix = XMMatrixMultiply(ViewProject, HalfLambert);

	FViewportTransformation ViewportTransformation;
	XMStoreFloat4x4(&ViewportTransformation.ViewProjectionMatrix, XMMatrixTranspose(ViewProject));
	XMStoreFloat4x4(&ViewportTransformation.TexViewProjectionMatrix, XMMatrixTranspose(TexViewProjectionMatrix));

	// 拿到视口位置
	// ビューポート位置を取得
	ViewportTransformation.ViewportPosition = ViewportInfo.ViewPosition;

	ViewportConstantBufferViews.Update(InConstantBufferOffset, &ViewportTransformation);
}



void FGeometryMap::UpdateMaterialShaderResourceView(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	FMaterialConstantBuffer MaterialConstantBuffer;
	for (size_t i = 0; i < Materials.size(); i++)
	{
		// 变换材质
		// マテリアルを変換
		if (CMaterial* InMaterial = Materials[i])
		{
			if (InMaterial->IsDirty())
			{
				// BaseColor
				MaterialConstantBuffer.BaseColor = EngineMath::ToFloat4(InMaterial->GetBaseColor());

				// 高光颜色
				// スペキュラカラー
				MaterialConstantBuffer.SpecularColor = EngineMath::ToFloat3(InMaterial->GetSpecularColor());

				// 粗糙度
				// 粗さ
				MaterialConstantBuffer.Roughness = InMaterial->GetRoughness();

				// 折射率
				// 屈折率
				MaterialConstantBuffer.Refraction = InMaterial->GetRefractiveValue();

				// 类型输入
				// タイプ入力
				MaterialConstantBuffer.MaterialType = InMaterial->GetMaterialType();

				// F0输入
				// F0入力
				fvector_3d F0 = InMaterial->GetFresnelF0();
				MaterialConstantBuffer.FresnelF0 = XMFLOAT3(F0.x, F0.y, F0.z);

				// 透明度
				MaterialConstantBuffer.Transparency = InMaterial->GetTransparency();

				// 金属度
				// メタリック度
				MaterialConstantBuffer.Metallicity = EngineMath::ToFloat3(InMaterial->GetMetallicity());

				// 外部资源导入
				// 外部リソースのインポート
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
					// 法線
					if (auto NormalTextureResourcesPtr = FindRenderingTexture(InMaterial->GetNormalIndexKey()))
					{
						MaterialConstantBuffer.NormalIndex = (*NormalTextureResourcesPtr)->RenderingTextureID;
					}
					else
					{
						MaterialConstantBuffer.NormalIndex = -1;
					}


					// 高光
					// ハイライト
					if (auto SpecularTextureResourcesPtr = FindRenderingTexture(InMaterial->GetSpecularKey()))
					{
						MaterialConstantBuffer.SpecularIndex = (*SpecularTextureResourcesPtr)->RenderingTextureID;
					}
					else
					{
						MaterialConstantBuffer.SpecularIndex = -1;
					}
				}

				// 材质矩阵
				// マテリアル行列
				XMMATRIX MaterialTransform = XMLoadFloat4x4(&InMaterial->GetMaterialTransform());
				XMStoreFloat4x4(&MaterialConstantBuffer.TransformInformation,
					XMMatrixTranspose(MaterialTransform));

				InMaterial->SetDirty(false);

				// 自定义项
				// 後で追加
				// float
				MaterialConstantBuffer.Param0 = InMaterial->GetFloatParam(0);
				MaterialConstantBuffer.Param1 = InMaterial->GetFloatParam(1);
				MaterialConstantBuffer.Param2 = InMaterial->GetFloatParam(2);

				MaterialConstantBufferViews.Update(InMaterial->GetMaterialIndex(), &MaterialConstantBuffer);
			}
		}
	}
}

void FGeometryMap::UpdateLight(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	// 更新灯光
	// ライトを更新
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
					// 正投影行列
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
					// ハーフランバート
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
					//シャドウ変換情報を格納
					XMStoreFloat4x4(&LightConstantBuffer.SceneLights[i].ShadowTransform, XMMatrixTranspose(ShadowViewProjectMatrixRTX));

					break;
				}
				case ELightType::PointLight:
					if (CRangeLightComponent* InRangeLightComponent = dynamic_cast<CRangeLightComponent*>(InLightComponent))
					{
						LightConstantBuffer.SceneLights[i].StartAttenuation = InRangeLightComponent->GetStartAttenuation();
						LightConstantBuffer.SceneLights[i].EndAttenuation = InRangeLightComponent->GetEndAttenuation();
					}

					break;
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

					XMFLOAT3 ForwardVector = InLightComponent->GetForwardVector();
					XMFLOAT3 Position = InLightComponent->GetPosition();

					DynamicShadowMap.BuildSpotLightMatrix(
						EngineMath::ToVector3d(ForwardVector),
						EngineMath::ToVector3d(Position),
						370.f);

					XMFLOAT4X4 ShadowViewMatrix;
					XMFLOAT4X4 ShadowProjectMatrix;
					DynamicShadowMap.GetViewportMatrix(ShadowViewMatrix, ShadowProjectMatrix);

					XMMATRIX ShadowViewMatrixRTX = XMLoadFloat4x4(&ShadowViewMatrix);
					XMMATRIX ShadowProjectMatrixRTX = XMLoadFloat4x4(&ShadowProjectMatrix);

					// NDC [-1,1]; = >[0,1]
					// 半兰伯特
					// ハーフランバート
					XMMATRIX Transform =
					{
						0.5f, 0.0f, 0.0f, 0.0f,
						0.0f, -0.5f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.5f, 0.5f, 0.0f, 1.0f
					};

					XMMATRIX ShadowViewProjectMatrixRTX =
						ShadowViewMatrixRTX * ShadowProjectMatrixRTX * Transform;

					// 存储Shadow变换信息
					// シャドウ変換情報を格納
					XMStoreFloat4x4(&LightConstantBuffer.SceneLights[i].ShadowTransform, XMMatrixTranspose(ShadowViewProjectMatrixRTX));

					break;
				}
			}
		}
	}

	LightConstantBufferViews.Update(0, &LightConstantBuffer);
}
void FGeometryMap::UpdateFog(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	// 更新雾
	// フォグを更新
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
	// 构建普通阴影
	// 通常シャドウを構築
	DynamicShadowMap.Init(2048, 2048);

	DynamicShadowMap.BuildViewport(fvector_3d(0.f, 0.f, 0.f));

	DynamicShadowMap.BuildDepthStencilDescriptor();

	DynamicShadowMap.BuildRenderTargetDescriptor();

	// 构建点光源阴影
	// 点光源シャドウを構築
	DynamicShadowCubeMap.BuildViewport(fvector_3d(0.f, 0.f, 0.f));
	DynamicShadowCubeMap.BuildDepthStencilDescriptor();
	DynamicShadowCubeMap.BuildRenderTargetDescriptor();
	DynamicShadowCubeMap.BuildDepthStencil();
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

void FGeometryMap::DuplicateMesh(CMeshComponent* InMesh, const std::vector<FRenderingData>& MeshDataGroup)
{
	for (auto& Tmp : Geometrys)
	{
		Tmp.second.DuplicateMesh(InMesh, MeshDataGroup, Tmp.first);
	}
}

bool FGeometryMap::FindMeshRenderingDataByHash(const size_t& InHash, std::vector<FRenderingData>& MeshDataGroup, int InRenderLayerIndex)
{
	for (auto& Tmp : Geometrys)
	{
		if (Tmp.second.FindMeshRenderingDataByHash(InHash, MeshDataGroup, InRenderLayerIndex))
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

	// 获取资源路径
	// リソースパスを取得
	std::string AssetPath =
		FEnginePathHelper::RelativeToAbsolutePath(
			FEnginePathHelper::GetEngineAssetPath());

	find_files(AssetPath.c_str(), &Paths, true);

	for (int i = 0; i < Paths.index; i++)
	{
		if (find_string(Paths.paths[i], ".dds", 0) != -1 ||
			find_string(Paths.paths[i], ".DDS", 0) != -1)
		{
			//单位化路径
			//パスを正規化
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

	// 预加载场景当前实际引用到的材质贴图
	// シーンで現在使用されているマテリアルテクスチャをプリロードする
	for (auto& Tmp : GObjects)
	{
		if (CMeshComponent* InMeshComponent = dynamic_cast<CMeshComponent*>(Tmp))
		{
			if (auto InMaterials = InMeshComponent->GetMaterials())
			{
				for (CMaterial* InMaterial : *InMaterials)
				{
					if (InMaterial == nullptr)
					{
						continue;
					}

					if (!InMaterial->GetBaseColorIndexKey().empty())
					{
						RenderingTexture2DResources->FindRenderingTexture(InMaterial->GetBaseColorIndexKey());
					}

					if (!InMaterial->GetNormalIndexKey().empty())
					{
						RenderingTexture2DResources->FindRenderingTexture(InMaterial->GetNormalIndexKey());
					}

					if (!InMaterial->GetSpecularKey().empty())
					{
						RenderingTexture2DResources->FindRenderingTexture(InMaterial->GetSpecularKey());
					}
				}
			}
		}
	}
}

void FGeometryMap::Build()
{
	//构建模型
	//モデルを構築
	for (auto& Tmp : Geometrys)
	{
		Tmp.second.Build();
	}
}

void FGeometryMap::BuildDescriptorHeap()
{
	// 构建贴图描述符表
	// テクスチャ記述子テーブルを構築
	DescriptorHeap.Build(
		GetDrawTexture2DResourcesNumber() + //Texture2D
		GetDrawCubeMapResourcesNumber() + //静态Cube贴图 背景 天空球   // 静的Cubeマップ 背景 スカイボックス
		1 + //动态Cube贴图 反射                     // 動的Cubeマップ 反射
		1 + //Shadow 直射灯 聚光灯               	// シャドウ 直射光 スポットライト
		1 + //ShadowCubeMap 点光源的                // ShadowCubeMap 点光源のシャドウ
		1 + //UI
		1 + //法线                                  // 法線
		1 + //Depth
		1 + //Noise 
		1 + //SSAO
		1); //双边模糊                              // 両方向ブラー
}

void FGeometryMap::BuildMeshConstantBuffer()
{
	//创建常量缓冲区
	//定数バッファを作成
	MeshConstantBufferViews.CreateConstant(sizeof(FObjectTransform), GetDrawMeshObjectNumber());

}

void FGeometryMap::BuildFogConstantBuffer()
{
	// 创建雾常量缓冲区视图
    // フォグの定数バッファビューを作成
	FogConstantBufferViews.CreateConstant(sizeof(FFogConstantBuffer), 1);
}

void FGeometryMap::BuildMaterialShaderResourceView()
{
	Materials.clear();
	std::unordered_set<CMaterial*> UniqueMaterials;

	// 收集材质
	// 更新Shader-Index
	// マテリアルを収集
	// シェーダーインデックスを更新
	for (auto& Tmp : FRenderLayerManager::RenderLayers)
	{
		for (auto& InData : Tmp->RenderDatas)
		{
			if (!InData.expired())
			{
				if (auto InMaterials = InData.lock()->Mesh->GetMaterials())
				{
					for (size_t j = 0; j < InMaterials->size(); j++)
					{
						if (CMaterial* Material = (*InMaterials)[j])
						{
							if (UniqueMaterials.insert(Material).second)
							{
								Material->SetMaterialIndex(Materials.size());
								Materials.push_back(Material);
							}
						}
					}
				}
			}
		}
	}

	// 创建常量缓冲区
	// 定数バッファを作成
	MaterialConstantBufferViews.CreateConstant(
		sizeof(FMaterialConstantBuffer),
		GetDrawMaterialObjectNumber(),
		false);
}


void FGeometryMap::BuildLightConstantBuffer()
{
	// 创建常量缓冲区
	// 定数バッファを作成
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
	// 2Dテクスチャを構築
	RenderingTexture2DResources->BuildTextureConstantBuffer(
		DescriptorHeap.GetHeap(), 0);//视口     //ビューポート

	// 构建CubeMap贴图
	// CubeMapテクスチャを構築
	RenderingCubeMapResources->BuildTextureConstantBuffer(
		DescriptorHeap.GetHeap(),
		GetDrawTexture2DResourcesNumber());//加上2D贴图地址偏移 //2Dテクスチャのアドレスオフセットを追加
}

void FGeometryMap::BuildViewportConstantBufferView(UINT InViewportOffset)
{
	//****************!!!!新增视口时需检查此处常量缓冲区设置!!!*******************
	//!!!!新規ビューポート追加時はここで定数バッファ設定を確認すること!!!***
	
	//创建常量缓冲区
	// 定数バッファを作成
	ViewportConstantBufferViews.CreateConstant(sizeof(FViewportTransformation),
		1 + //主视口 摄像机                                         // メインビューポート カメラ
		GetDynamicReflectionViewportNum() + //动态反射的视口        // 動的反射用のビューポート
		1 + //阴影视口                                              // シャドウビューポート
		6 + //ShadowCubeMap(用于点光源阴影)                         //ShadowCubeMap（点光源用シャドウ）
		InViewportOffset);
}

UINT FGeometryMap::GetDynamicReflectionMeshComponentsSize()
{
	return DynamicReflectionMeshComponents.size();
}

CMeshComponent* FGeometryMap::GetDynamicReflectionMeshComponents(int Index)
{
	return DynamicReflectionMeshComponents[Index];
}

UINT FGeometryMap::GetViewportConstantBufferByteSize()
{
	return ViewportConstantBufferViews.GetConstantBufferByteSize();
}

D3D12_GPU_VIRTUAL_ADDRESS FGeometryMap::ViewportGPUVirtualAddress()
{
	return ViewportConstantBufferViews.GetBuffer()->GetGPUVirtualAddress();
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
	//找到对应层级
	//対応する階層を見つける
	if (std::shared_ptr<FRenderLayer> InRenderLayer = FRenderLayerManager::FindByRenderLayer((int)InMesh->GetRenderLayerType()))
	{
		FMeshRenderingData MeshDataWithSections = MeshData;
		MeshDataWithSections.BuildDefaultSectionIfNeeded();

		// 计算AABB包围盒
		// AABBバウンディングボックスを計算
		BoundingBox Bounds;
		{
			fvector_3d MaxPoint = fvector_3d(-FLT_MAX);
			fvector_3d MinPoint = fvector_3d(+FLT_MAX);
			for (auto& Tmp : MeshDataWithSections.VertexData)
			{

				MinPoint.x = math_libray::Min(Tmp.Position.x, MinPoint.x);
				MinPoint.y = math_libray::Min(Tmp.Position.y, MinPoint.y);
				MinPoint.z = math_libray::Min(Tmp.Position.z, MinPoint.z);

				MaxPoint.x = math_libray::Max(Tmp.Position.x, MaxPoint.x);
				MaxPoint.y = math_libray::Max(Tmp.Position.y, MaxPoint.y);
				MaxPoint.z = math_libray::Max(Tmp.Position.z, MaxPoint.z);
			}

			XMFLOAT3 XMFMaxPoint = EngineMath::ToFloat3(MaxPoint);
			XMFLOAT3 XMFMinPoint = EngineMath::ToFloat3(MinPoint);

			XMVECTOR XMFMaxPointTOR = XMLoadFloat3(&XMFMaxPoint);
			XMVECTOR XMFMinPointTOR = XMLoadFloat3(&XMFMinPoint);

			XMStoreFloat3(&Bounds.Center, (XMFMaxPointTOR + XMFMinPointTOR) * 0.5f);
			XMStoreFloat3(&Bounds.Extents, (XMFMaxPointTOR - XMFMinPointTOR) * 0.5f);
		}

		std::vector<FRenderingData>& CachedRenderingData = UniqueRenderingDatas[InMeshHash];
		CachedRenderingData.clear();

		const UINT BaseIndexOffset = (UINT)MeshRenderingData.IndexData.size();
		const UINT BaseVertexOffset = (UINT)MeshRenderingData.VertexData.size();

		for (const FMeshSection& Section : MeshDataWithSections.Sections)
		{
			RenderingDatas.push_back(std::make_shared<FRenderingData>());
			std::shared_ptr<FRenderingData> InRenderingData = RenderingDatas[RenderingDatas.size() - 1];

			InRenderLayer->RenderDatas.push_back(InRenderingData);

			InRenderingData->MeshObjectIndex = MeshObjectCount++;
			InRenderingData->Mesh = InMesh;
			InRenderingData->MeshHash = InMeshHash;
			InRenderingData->GeometryKey = InKey;
			InRenderingData->MaterialSlotIndex = (int)Section.MaterialSlotIndex;

			InRenderingData->IndexSize = Section.IndexSize;
			InRenderingData->VertexSize = Section.VertexSize;

			InRenderingData->IndexOffsetPosition = BaseIndexOffset + Section.IndexOffset;
			InRenderingData->VertexOffsetPosition = BaseVertexOffset + Section.VertexOffset;

			InRenderingData->MeshRenderingData = &MeshRenderingData;
			InRenderingData->Bounds = Bounds;

			CachedRenderingData.push_back(*InRenderingData);
		}

		// 合并索引
		// インデックスを統合
		MeshRenderingData.IndexData.insert(
			MeshRenderingData.IndexData.end(),
			MeshDataWithSections.IndexData.begin(),
			MeshDataWithSections.IndexData.end());

		// 合并顶点
		// 頂点を統合
		MeshRenderingData.VertexData.insert(
			MeshRenderingData.VertexData.end(),
			MeshDataWithSections.VertexData.begin(),
			MeshDataWithSections.VertexData.end());
	}
}


void FGeometry::DuplicateMesh(CMeshComponent* InMesh, const std::vector<FRenderingData>& MeshDataGroup, int InKey)
{
	if (std::shared_ptr<FRenderLayer> InRenderLayer = FRenderLayerManager::FindByRenderLayer((int)InMesh->GetRenderLayerType()))
	{
		for (const FRenderingData& CachedData : MeshDataGroup)
		{
			RenderingDatas.push_back(std::make_shared<FRenderingData>());
			std::shared_ptr<FRenderingData> InRenderingData = RenderingDatas[RenderingDatas.size() - 1];

			InRenderLayer->RenderDatas.push_back(InRenderingData);

			InRenderingData->Mesh = InMesh;
			InRenderingData->MeshObjectIndex = MeshObjectCount++;
			InRenderingData->GeometryKey = InKey;
			InRenderingData->MaterialSlotIndex = CachedData.MaterialSlotIndex;

			InRenderingData->MeshHash = CachedData.MeshHash;

			InRenderingData->IndexSize = CachedData.IndexSize;
			InRenderingData->VertexSize = CachedData.VertexSize;

			InRenderingData->IndexOffsetPosition = CachedData.IndexOffsetPosition;
			InRenderingData->VertexOffsetPosition = CachedData.VertexOffsetPosition;

			InRenderingData->MeshRenderingData = &MeshRenderingData;
			InRenderingData->Bounds = CachedData.Bounds;
		}
	}
}

bool FGeometry::IsRenderingDataExistence(CMeshComponent* InKey)
{
	if (std::shared_ptr<FRenderLayer> InRenderLayer = FRenderLayerManager::FindByRenderLayer((int)InKey->GetRenderLayerType()))
	{
		for (auto& Tmp : InRenderLayer->RenderDatas)
		{
			if (!Tmp.expired())
			{
				if (Tmp.lock()->Mesh == InKey)
				{
					return true;
				}
			}

		}
	}

	return false;
}
bool FGeometry::FindMeshRenderingDataByHash(const size_t& InHash, std::vector<FRenderingData>& MeshDataGroup, int InRenderLayerIndex)
{
	auto FindElement = UniqueRenderingDatas.find(InHash);
	if (FindElement != UniqueRenderingDatas.end())
	{
		MeshDataGroup = FindElement->second;

		return true;
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

void FGeometry::FindRenderingDatas(std::function<EFindValueType(std::shared_ptr<FRenderingData>&)> InFun)
{
	for (auto& Tmp : RenderingDatas)
	{
		if (InFun(Tmp) == EFindValueType::TYPE_COMPLETE)
		{
			break;
		}
	}
}

