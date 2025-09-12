#include "DirectXRenderingEngine.h"

#include "../../../../Debug/EngineDebug.h"
#include "../../../../Config/EngineRenderConfig.h"
#include "../../../../Rendering/Core/Rendering.h"

#include "../../../../Mesh/BoxMesh.h"
#include "../../../../Mesh/SphereMesh.h"
#include "../../../../Mesh/CylinderMesh.h"
#include "../../../../Mesh/ConeMesh.h"
#include "../../../../Mesh/PlaneMesh.h"
#include "../../../../Mesh/CustomMesh.h"
#include "../../../../Mesh/PyramidMesh.h"
#include "../../../../Mesh/PipeMesh.h"
#include "../../../../Mesh/TorusMesh.h"

#include "../../../../Core/CoreObject/CoreMinimalObject.h"
#include "../../../../Core/World.h"
#include "../../../../Mesh/Core/MeshManager.h"
#include "../../../../Mesh/Core/Material/Material.h"
#include "../../../../Component/Mesh/Core/MeshComponent.h"

// 视口
#include "../../../../Core/World.h"
#include "../../../../Core/Camera.h"
// 天空
#include "../../../../Actor/Sky/Fog.h"
#include "../../../../Actor/Sky/Sky.h"

// 灯光相关
#include "../../../../Manager/LightManager.h"
#include "../../../../Actor/Light/ParallelLight.h"
#include "../../../../Actor/Light/SpotLight.h"
#include "../../../../Actor/Light/PointLight.h"

#if defined(_WIN32)
#include "../../../../Core/WinMainCommandParameters.h"

// 添加编辑器界面移动箭头支持
#if EDITOR_ENGINE

#include "../../../../../EditorEngine/SelectEditor/OperationHandle/MoveArrow.h"
#include "../../../../../EditorEngine/SelectEditor/OperationHandle/RotatorArrow.h"
#include "../../../../../EditorEngine/SelectEditor/OperationHandle/ScalingArrow.h"
#endif


// 声明控制手柄
extern GMoveArrow* MoveArrow;
extern GScalingArrow* ScalingArrow;
extern GRotatorArrow* RotatorArrow;


CDirectXRenderingEngine::CDirectXRenderingEngine()
	:CurrentFenceIndex(0)
	, M4XQualityLevels(1)
	, bMSAA4XEnabled(false)
	, BackBufferFormat(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM)
	, DepthStencilFormat(DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT)
	, CurrentSwapBuffIndex(0)
{
	for (int i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
	{
		SwapChainBuffer.push_back(ComPtr<ID3D12Resource>());
	}

	bTick = false;

	FCreateObjectParam Param;
	Param.Outer = this;
	MeshManager = CreateObject<CMeshManager>(Param, new CMeshManager());
	LightManager = CreateObject<CLightManager>(Param, new CLightManager());
}

CDirectXRenderingEngine::~CDirectXRenderingEngine()
{
	delete MeshManager;
	delete LightManager;
}

int CDirectXRenderingEngine::PreInit(FWinMainCommandParameters InParameters)
{
	Engine_Log("DirectXRenderingEngine pre initialization complete.");
	return 0;
}

int CDirectXRenderingEngine::Init(FWinMainCommandParameters InParameters)
{
	InitDirect3D();

	PostInitDirect3D();

	MeshManager->Init();

	Engine_Log("DirectXRenderingEngine initialization complete.");

	return 0;
}

int CDirectXRenderingEngine::PostInit()
{
	Engine_Log("Engine post initialization complete.");

	ANALYSIS_HRESULT(GraphicsCommandList->Reset(CommandAllocator.Get(), NULL));
	{
		// 创建控制手柄
#if EDITOR_ENGINE
		if (GMoveArrow* InMoveArrow = World->CreateActorObject<GMoveArrow>())
		{
			InMoveArrow->CreateMesh();

			MoveArrow = InMoveArrow;
		}
		if (GScalingArrow* InScalingArrow = World->CreateActorObject<GScalingArrow>())
		{
			InScalingArrow->CreateMesh();

			ScalingArrow = InScalingArrow;
		}

		if (GRotatorArrow* InRotatorArrow = World->CreateActorObject<GRotatorArrow>())
		{
			InRotatorArrow->CreateMesh();

			RotatorArrow = InRotatorArrow;
		}
#endif
		// 创建灯光
		
		////  聚光灯
		//if (GSpotLight* SpotLight = World->CreateActorObject<GSpotLight>())
		//{
		//	SpotLight->SetPosition(XMFLOAT3(0.f, 0.f, 20.f));
		//	SpotLight->SetRotation(fvector_3d(0.f, 0.f, 0.f));

		//	SpotLight->SetLightIntensity(fvector_3d(1.3f, 1.3f, 1.3f));
		//	//SpotLight->SetStartAttenuation(1.f);
		//	SpotLight->SetEndAttenuation(130.f);

		//	SpotLight->SetConicalInnerCorner(40.f);
		//	SpotLight->SetConicalOuterCorner(60.f);
		//}
		//// 点光源
		//if (GPointLight* PointLight = World->CreateActorObject<GPointLight>())
		//{
		//	PointLight->SetPosition(XMFLOAT3(0.f, 10.f, 10.f));
		//	PointLight->SetRotation(fvector_3d(0.f, 0.f, 0.f));

		//	PointLight->SetLightIntensity(fvector_3d(0.9f));
		//	PointLight->SetEndAttenuation(190.f);
		//}
		// 平行光
		if (GParallelLight* ParallelLight = World->CreateActorObject<GParallelLight>())
		{
			ParallelLight->SetPosition(XMFLOAT3(10.f, -10.f, 10.f));
			ParallelLight->SetRotation(fvector_3d(30.f, 0.f, 0.f));
			ParallelLight->SetScale(fvector_3d(1));
			ParallelLight->SetLightIntensity(fvector_3d(1.1f, 1.1f, 1.1f));

		}
	
		// 地板
		if (GPlaneMesh* InPlaneMesh = World->CreateActorObject<GPlaneMesh>())
		{
			InPlaneMesh->CreateMesh(4.f, 3.f, 20, 20);

			InPlaneMesh->SetPosition(XMFLOAT3(0.f, -12.f, 0.f));
			InPlaneMesh->SetScale(fvector_3d(50.f, 1.f, 50.f));
			// 地面不显示阴影
			InPlaneMesh->SetCastShadow(false);
			InPlaneMesh->SetPickup(false);
			if (CMaterial* InMaterial = (*InPlaneMesh->GetMaterials())[0])
			{
				InMaterial->SetMaterialType(EMaterialType::Lambert);
			}

		}

		//甜甜圈
		if (GTorusMesh* InTorusMesh = World->CreateActorObject<GTorusMesh>())
		{
			InTorusMesh->CreateMesh(6.f, 2.f, 40.f, 40.f);
			InTorusMesh->SetPosition(XMFLOAT3(-22.f, -8, 20.f));
			InTorusMesh->SetScale(fvector_3d(1.f));
			if (CMaterial* InMaterial = (*InTorusMesh->GetMaterials())[0])
			{
				InMaterial->SetMaterialType(EMaterialType::HalfLambert);

				InMaterial->SetBaseColor("Zhuan");
				InMaterial->SetNormal("Zhuan_NRM");
			}
		}

		//棱锥
		if (GPyramidMesh* InPyramidMesh = World->CreateActorObject<GPyramidMesh>())
		{
			InPyramidMesh->CreateMesh(EPyramidNumberSides::Pyramid_3, 1);
			InPyramidMesh->SetPosition(XMFLOAT3(-1.f, -8, 20.f));
			InPyramidMesh->SetRotation(fvector_3d(0.f, 90.f, 0.f));
			InPyramidMesh->SetScale(fvector_3d(1.f));
			if (CMaterial* InMaterial = (*InPyramidMesh->GetMaterials())[0])
			{
				InMaterial->SetMaterialType(EMaterialType::HalfLambert);
			}
		}

		//Pipe模型
		if (GPipeMesh* InPipeMesh = World->CreateActorObject<GPipeMesh>())
		{
			InPipeMesh->CreateMesh(3.f, 3.f, 6.f, 1.f, 20.f, 20.f);
			InPipeMesh->SetPosition(XMFLOAT3(-9.f, -9, 20.f));
			InPipeMesh->SetScale(fvector_3d(1.f));
			if (CMaterial* InMaterial = (*InPipeMesh->GetMaterials())[0])
			{
				//InMaterial->SetBaseColor(fvector_4d(5.f));
				//InMaterial->SetMaterialDisplayStatus(EMaterialDisplayStatusType::WireframeDisplay);
				InMaterial->SetMaterialType(EMaterialType::HalfLambert);

				InMaterial->SetBaseColor("Zhuan");
				InMaterial->SetNormal("Zhuan_NRM");
			}
		}

		//锥形
		if (GConeMesh* InConeMesh = World->CreateActorObject<GConeMesh>())
		{
			InConeMesh->CreateMesh(2.f, 3.f, 20, 20);

			InConeMesh->SetPosition(XMFLOAT3(7.f, -11.f, 20.f));
			InConeMesh->SetScale(fvector_3d(1.f, 1.f, 1.f));
			if (CMaterial* InMaterial = (*InConeMesh->GetMaterials())[0])
			{
				//InMaterial->SetBaseColor(fvector_4d(1.f));
				//	InMaterial->SetBaseColor(fvector_4d(1.f));
				InMaterial->SetMaterialType(EMaterialType::HalfLambert);
			}
		}

		if (GBoxMesh* InBoxMesh = World->CreateActorObject<GBoxMesh>())
		{
			InBoxMesh->CreateMesh(5.f, 5.f, 5.f);

			InBoxMesh->SetPosition(XMFLOAT3(22.f, -10.f, 20.f));
			InBoxMesh->SetScale(fvector_3d(1.f, 1.f, 1.f));
			if (CMaterial* InMaterial = (*InBoxMesh->GetMaterials())[0])
			{
				//InMaterial->SetBaseColor(fvector_4d(1.f));
				//	InMaterial->SetBaseColor(fvector_4d(0.5f));
				InMaterial->SetMaterialType(EMaterialType::HalfLambert);
			}
		}

		if (GCylinderMesh* InCylinderMesh = World->CreateActorObject<GCylinderMesh>())
		{
			InCylinderMesh->CreateMesh(2.f, 2.f, 5.f, 20.f, 20.f);

			InCylinderMesh->SetPosition(XMFLOAT3(14.f, -10.f, 20.f));
			InCylinderMesh->SetScale(fvector_3d(1.f));
			if (CMaterial* InMaterial = (*InCylinderMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor("Zhuan");
				InMaterial->SetNormal("Zhuan_NRM");

				InMaterial->SetMaterialType(EMaterialType::HalfLambert);
			}
		}

	
		//兰伯特
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(-3.f, 2, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetMaterialType(EMaterialType::Lambert);
			}
		}

		//半兰伯特
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(-3.f, 8, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(
					221.f / 255.f,
					154.f / 255.f,
					255.f / 255.f, 1.f));

				InMaterial->SetMaterialType(EMaterialType::HalfLambert);
			}
		}
		// wrap
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(-3.f, 14, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(
					234.f / 255.f,
					154.f / 255.f,
					139.f / 255.f, 1.f));

				InMaterial->SetMaterialType(EMaterialType::Wrap);
			}
		}
		// Minnaert
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(-9.f, 2, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(
					0.9f,
					0.9f,
					1.0f, 1.f));

				InMaterial->SetMaterialType(EMaterialType::Minnaert);

				InMaterial->SetRoughness(0.95f);
			}
		}
		// Banded
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(3.f, 2, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(
					0.7f,
					0.7f,
					1.4f, 1.f));
				InMaterial->SetMaterialType(EMaterialType::Banded);

			}
		}
		// Gradual Banded
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(3.f, 8, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(
					247.f / 255.f,
					150.f / 255.f,
					85.f / 255.f, 1.f));
				InMaterial->SetMaterialType(EMaterialType::GradualBanded);

			}
		}
		// Final Banded
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(3.f, 14, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(
					213.f / 255.f,
					132.f / 255.f,
					234.f / 255.f, 1.f));

				InMaterial->SetMaterialType(EMaterialType::FinalBanded);

				InMaterial->SetRoughness(0.6f);
			}
		}
		//phong 
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(9.f, 2, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				//模拟黄铜
				InMaterial->SetBaseColor(fvector_4d(
					191.f / 255.f,
					173.f / 255.f,
					111.f / 255.f, 1.f));

				InMaterial->SetMaterialType(EMaterialType::Phong);
				InMaterial->SetSpecular(fvector_3d(1.f));

				InMaterial->SetFresnelF0(fvector_3d(0.08f));
				InMaterial->SetRoughness(0.4f);
			}
		}
		// Fresnel
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(9.f, 8, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(0.5f, 0.5f, 0.5f, 1.f));
				InMaterial->SetMaterialType(EMaterialType::Fresnel);

				InMaterial->SetRoughness(0.9f);
			}
		}

		// binn phong 
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			//SphereMesh->SetMeshRenderLayerType(EMeshRenderLayerType::RENDERLAYER_OPAQUE_REFLECTOR);

			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(9.f, 14, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(
					220.f / 255.f,
					223.f / 255.f,
					227.f / 255.f, 1.f));

				InMaterial->SetMaterialType(EMaterialType::BinnPhong);
				InMaterial->SetSpecular(fvector_3d(1.f));
				InMaterial->SetRoughness(0.3f);

				InMaterial->SetFresnelF0(fvector_3d(0.1f));
				InMaterial->SetDynamicReflection(true);
			}
		}

		// back 透射材质
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(15.f, 2, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(
					2.f / 255.f,
					214.f / 255.f,
					17.f / 255.f, 
					1.f));
				InMaterial->SetMaterialType(EMaterialType::Back);

				InMaterial->SetRoughness(0.2f);
			}
		}
		// 各向异性 Kajiya-Kay Shading Model（卡吉雅模型）
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(15.f, 8, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor("Hair");
				
				InMaterial->SetMaterialType(EMaterialType::AnisotropyKajiyaKay);

			}
		}
		// OrenNayar
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(15.f, 14, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(
					0.7f,
					0.7f,
					1.4f, 1.f));
				InMaterial->SetMaterialType(EMaterialType::OrenNayar);

				InMaterial->SetRoughness(0.7f);//如果是0 就是兰伯特

			}
		}
	
	/*	if (GetCurrentGPU() == NVIDIA)
		{*/
			////以线框显示
			//if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
			//{
			//	SphereMesh->CreateMesh(2.f, 50, 50);
			//	SphereMesh->SetPosition(XMFLOAT3(9.f, 18, 0.f));
			//	if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			//	{
			//		InMaterial->SetMaterialDisplayStatus(EMaterialDisplayStatusType::WireframeDisplay);
			//		InMaterial->SetMaterialType(EMaterialType::BaseColor);
			//		InMaterial->SetBaseColor(fvector_4d(1.f, 1.f, 1.f, 1.f));
			//	}
			//}

			////以点显示
			//if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
			//{
			//	SphereMesh->SetMeshRenderLayerType(EMeshRenderLayerType::RENDERLAYER_WIREFRAME);

			//	SphereMesh->CreateMesh(2.f, 50, 50);
			//	SphereMesh->SetPosition(XMFLOAT3(-3.f, 18, 0.f));
			//	if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			//	{
			//		InMaterial->SetMaterialDisplayStatus(EMaterialDisplayStatusType::PointDisplay);
			//		InMaterial->SetMaterialType(EMaterialType::BaseColor);
			//		InMaterial->SetBaseColor(fvector_4d(1.f, 1.f, 1.f, 1.f));
			//	}
			//}
		//}
		
		// 以法线显示
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(21.f, 14, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetMaterialType(EMaterialType::Normal);
			}
		}
		// 以世界法线显示
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(21.f, 20, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
			    InMaterial->SetMaterialType(EMaterialType::WorldNormal);
			}
		}
		//法线木材
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(-9.f, -3, 0.f));
			SphereMesh->SetRotation(fvector_3d(0.f, -90.f, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor("Wood");
				InMaterial->SetNormal("Wood_NRM");
				InMaterial->SetBaseColor(fvector_4d(1.f));
				InMaterial->SetRoughness(4.f);
				InMaterial->SetMaterialType(EMaterialType::OrenNayar);
			}
		}

		//法线地面效果
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(-3.f, -3, 0.f));
			SphereMesh->SetRotation(fvector_3d(0.f, -90.f, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor("MMOARPG");
				InMaterial->SetNormal("MMOARPG_NRM");
				InMaterial->SetMaterialType(EMaterialType::OrenNayar);
			}
		}

		// 贴图反射
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->SetMeshRenderLayerType(EMeshRenderLayerType::RENDERLAYER_OPAQUE_REFLECTOR);

			SphereMesh->CreateMesh(2.f, 50, 50);
			SphereMesh->SetPosition(XMFLOAT3(3.f, -3, 0.f));
			SphereMesh->SetRotation(fvector_3d(0.f, -90.f, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetDynamicReflection(true);

				InMaterial->SetBaseColor("Earth");
				InMaterial->SetSpecular(fvector_3d(1.f));
				InMaterial->SetMaterialType(EMaterialType::BinnPhong);


				InMaterial->SetFresnelF0(fvector_3d(0.1f));
				InMaterial->SetRoughness(0.1f);
			}
		}
		// 单独法线贴图
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 100, 100);
			SphereMesh->SetPosition(XMFLOAT3(9.f, -3, 0.f));
			SphereMesh->SetRotation(fvector_3d(0.f, 90.f, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetNormal("Wood2_Nor");
				//InMaterial->SetBaseColor("Wood2");
				InMaterial->SetMaterialType(EMaterialType::BinnPhong);
			}
		}
		// 单独法线贴图+反射贴图
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 100, 100);
			SphereMesh->SetPosition(XMFLOAT3(15.f, -3, 0.f));
			SphereMesh->SetRotation(fvector_3d(0.f, 90.f, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetNormal("Wood2_Nor");
				//InMaterial->SetBaseColor("Wood2");
				InMaterial->SetSpecular("Wood2_SPEC");
				InMaterial->SetMaterialType(EMaterialType::BinnPhong);
			}
		}
		// PBR模型
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->CreateMesh(2.f, 100, 100);
			SphereMesh->SetPosition(XMFLOAT3(21.f, 2, 0.f));
			SphereMesh->SetRotation(fvector_3d(0.f, 0.f, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(1.f));
				InMaterial->SetMaterialType(EMaterialType::PBR);
			}
		}
		// 透明
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->SetMeshRenderLayerType(EMeshRenderLayerType::RENDERLAYER_TRANSPARENT);

			SphereMesh->CreateMesh(2.f, 100, 100);
			SphereMesh->SetPosition(XMFLOAT3(21.f, 7, 0.f));
			SphereMesh->SetRotation(fvector_3d(0.f, 0.f, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor("WireFence");
				InMaterial->SetMaterialType(EMaterialType::HalfLambert);
			}
		}
	
		// 反射球
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->SetMeshRenderLayerType(EMeshRenderLayerType::RENDERLAYER_OPAQUE_REFLECTOR);
			SphereMesh->CreateMesh(2.f, 100, 100);
			SphereMesh->SetPosition(XMFLOAT3(21.f, 2, 0.f));
			SphereMesh->SetRotation(fvector_3d(0.f, 0.f, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetDynamicReflection(true);
				InMaterial->SetBaseColor(fvector_4d(1.f));
				InMaterial->SetMaterialType(EMaterialType::Phong);

				InMaterial->SetRoughness(0.01f);
				InMaterial->SetFresnelF0(fvector_3d(0.5f));
			}
		}

		// 透明球
		if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())
		{
			SphereMesh->SetMeshRenderLayerType(EMeshRenderLayerType::RENDERLAYER_OPAQUE_REFLECTOR);
			SphereMesh->CreateMesh(2.f, 100, 100);
			SphereMesh->SetPosition(XMFLOAT3(0.f, 9, 10.f));
			//SphereMesh->SetPosition(XMFLOAT3(15.f, 18, 0.f));
			SphereMesh->SetRotation(fvector_3d(0.f, 0.f, 0.f));
			if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(0.f));
				InMaterial->SetMaterialType(EMaterialType::Transparency);

				InMaterial->SetRoughness(0.01f);
				InMaterial->SetFresnelF0(fvector_3d(0.5f));
				InMaterial->SetTransparency(1.f);
				InMaterial->SetDynamicReflection(true);

				InMaterial->SetSpecular(fvector_3d(1.f));
				InMaterial->SetRefractiveValue(1.11f);
			}
		}

		////PBR模型组
		//{
		//	//自由设定
		//	if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())//PBR模型
		//	{
		//		SphereMesh->CreateMesh(2.f, 30, 30);
		//		SphereMesh->SetPosition(XMFLOAT3(15.f, 2, 0.f));
		//		SphereMesh->SetRotation(fvector_3d(0.f, 0.f, 0.f));
		//		if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
		//		{
		//			InMaterial->SetBaseColor(fvector_4d(1.f));
		//			InMaterial->SetMaterialType(EMaterialType::PBR);
		//		}
		//	}

		//	int Row = 6;
		//	int Colum = 6;
		//	for (int i = 0; i < Colum; i++)
		//	{
		//		for (int j = 0; j < Row; j++)
		//		{
		//			fvector_3d PBRPosition(25.f, -3.f, 5.f);
		//			PBRPosition.y += i * 5.f;
		//			PBRPosition.z += j * 5.f;

		//			if (GSphereMesh* SphereMesh = World->CreateActorObject<GSphereMesh>())//PBR模型
		//			{
		//				SphereMesh->CreateMesh(2.f, 30, 30);
		//				SphereMesh->SetPosition(XMFLOAT3(PBRPosition.x, PBRPosition.y, PBRPosition.z));
		//				SphereMesh->SetRotation(fvector_3d(0.f, 0.f, 0.f));
		//				if (CMaterial* InMaterial = (*SphereMesh->GetMaterials())[0])
		//				{
		//					InMaterial->SetBaseColor(fvector_4d(1.f));
		//					InMaterial->SetMaterialType(EMaterialType::PBR);

		//					InMaterial->SetRoughness(((float)j + 1.f) / (float)Row);
		//					InMaterial->SetMetallicity(((float)i + 1.f) / (float)Colum);
		//				}
		//			}
		//		}
		//	}
		//}

		// 阴影shader单独显示
		if (GPlaneMesh* InPlaneMesh = World->CreateActorObject<GPlaneMesh>())
		{
			InPlaneMesh->CreateMesh(7.f, 7.f, 2, 2);
			InPlaneMesh->SetPosition(XMFLOAT3(0.f, 0.f, 60.f));
			InPlaneMesh->SetRotation(fvector_3d(90.f, 0.f, 0.f));
			if (CMaterial* InMaterial = (*InPlaneMesh->GetMaterials())[0])
			{
				InMaterial->SetMaterialType(ShadowTexture);
			}
		}
		// 外部FBX模型
		if (GCustomMesh* CustomMesh = World->CreateActorObject<GCustomMesh>())//反射球
		{
			string MeshPath = FEnginePathHelper::GetEngineAssetPath() + "/SK_Mannequin.FBX";
			CustomMesh->CreateMesh(MeshPath);
		
			CustomMesh->SetPosition(XMFLOAT3(0.f, 0, 40.f));
			CustomMesh->SetRotation(fvector_3d(0.f, 180.f, 0.f));
			// 是否渲染ShadowMap
			CustomMesh->SetCastShadow(false);
			if (CMaterial* InMaterial = (*CustomMesh->GetMaterials())[0])
			{
				InMaterial->SetBaseColor(fvector_4d(1.f));
				InMaterial->SetMaterialType(EMaterialType::HalfLambert);
		
			}
		}
	
		////well
		//if (GBoxMesh* InBoxMesh = World->CreateActorObject<GBoxMesh>())
		//{
		//	InBoxMesh->CreateMesh(30.f, 150, 0.4f);

		//	InBoxMesh->SetPosition(XMFLOAT3(0.f, 0.f, -60.f));
		//	if (CMaterial* InMaterial = (*InBoxMesh->GetMaterials())[0])
		//	{
		//		InMaterial->SetBaseColor(fvector_4d(1.f));
		//		InMaterial->SetMaterialType(EMaterialType::Lambert);
		//	}
		//}

		////well
		//if (GBoxMesh* InBoxMesh = World->CreateActorObject<GBoxMesh>())
		//{
		//	InBoxMesh->CreateMesh(30.f, 0.5f, 150.f);

		//	InBoxMesh->SetPosition(XMFLOAT3(70.f, 0.f, 0.f));
		//	if (CMaterial* InMaterial = (*InBoxMesh->GetMaterials())[0])
		//	{
		//		InMaterial->SetBaseColor(fvector_4d(1.f));
		//		InMaterial->SetMaterialType(EMaterialType::Lambert);
		//	}
		//}

		


		// 天空
		if (GSky* InSky = World->CreateActorObject<GSky>())//ﾌ・ﾕ
		{
			InSky->SetPosition(XMFLOAT3(0.f, 0.f, 0.f));
		}

		// 雾
		if (GFog* Fog = World->CreateActorObject<GFog>())
		{
			Fog->SetFogColor(fvector_color(0.7f, 0.7f, 0.9f, 1.f));
			Fog->SetFogStart(10.f);
			Fog->SetFogRange(500.f);

			Fog->SetFogHeight(5000.f);
			Fog->SetFogTransparentCoefficient(0.00f);

		}
	}

	MeshManager->BuildMesh();

	ANALYSIS_HRESULT(GraphicsCommandList->Close());

	ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);

	WaitGPUCommandQueueComplete();

	return 0;
}

void CDirectXRenderingEngine::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	MeshManager->UpdateCalculations(DeltaTime, ViewportInfo);
}

void CDirectXRenderingEngine::Tick(float DeltaTime)
{
	//重置录制相关的内存，为下一帧做准备
	ANALYSIS_HRESULT(CommandAllocator->Reset());

	MeshManager->PreDraw(DeltaTime);

	StartSetMainViewportRenderTarget();

	MeshManager->Draw(DeltaTime);
	MeshManager->PostDraw(DeltaTime);

	EndSetMainViewportRenderTarget();

	//录入完成
	ANALYSIS_HRESULT(GraphicsCommandList->Close());

	//提交命令
	ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);

	//交换两个buff缓冲区
	ANALYSIS_HRESULT(SwapChain->Present(0, 0));
	CurrentSwapBuffIndex = !(bool)CurrentSwapBuffIndex;

	//CPU等GPU
	WaitGPUCommandQueueComplete();
}

int CDirectXRenderingEngine::PreExit()
{
	Engine_Log("Engine post exit complete.");
	return 0;
}

int CDirectXRenderingEngine::Exit()
{

	Engine_Log("Engine exit complete.");
	return 0;
}

int CDirectXRenderingEngine::PostExit()
{
	FEngineRenderConfig::Destroy();


	// 退出前检查是否有未释放的资源
	ComPtr<ID3D12DebugDevice> debugDevice;
	if (SUCCEEDED(D3dDevice->QueryInterface(IID_PPV_ARGS(&debugDevice))))
	{
		debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	}

	Engine_Log("Engine post exit complete.");
	return 0;
}

void CDirectXRenderingEngine::StartSetMainViewportRenderTarget()
{
	// 转换资源状态
	CD3DX12_RESOURCE_BARRIER ResourceBarrierPresent = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentSwapBuff(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	GraphicsCommandList->ResourceBarrier(1, &ResourceBarrierPresent);

	// 需要每帧执行
	// 绑定矩形框
	GraphicsCommandList->RSSetViewports(1, &World->GetCamera()->ViewprotInfo);
	GraphicsCommandList->RSSetScissorRects(1, &World->GetCamera()->ViewprotRect);

	// 输出的合并阶段
	D3D12_CPU_DESCRIPTOR_HANDLE SwapBufferView = GetCurrentSwapBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView = GetCurrentDepthStencilView();
	GraphicsCommandList->OMSetRenderTargets(1, &SwapBufferView,
		true, &DepthStencilView);
}

void CDirectXRenderingEngine::EndSetMainViewportRenderTarget()
{
	CD3DX12_RESOURCE_BARRIER ResourceBarrierPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentSwapBuff(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	GraphicsCommandList->ResourceBarrier(1, &ResourceBarrierPresentRenderTarget);
}

void CDirectXRenderingEngine::ClearMainSwapChainCanvas()
{
	//清除画布
	GraphicsCommandList->ClearRenderTargetView(GetCurrentSwapBufferView(),
		DirectX::Colors::Black,
		0, nullptr);

	//清除深度模板缓冲区
	GraphicsCommandList->ClearDepthStencilView(GetCurrentDepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.f, 0, 0, NULL);
}
ID3D12Resource* CDirectXRenderingEngine::GetCurrentSwapBuff() const
{
	return SwapChainBuffer[CurrentSwapBuffIndex].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE CDirectXRenderingEngine::GetCurrentSwapBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		RTVHeap->GetCPUDescriptorHandleForHeapStart(),
		CurrentSwapBuffIndex, RTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE CDirectXRenderingEngine::GetCurrentDepthStencilView() const
{
	return DSVHeap->GetCPUDescriptorHandleForHeapStart();
}

CurrentGPU CDirectXRenderingEngine::GetCurrentGPU()
{
	IDXGIAdapter* pAdapter = nullptr;
	for (UINT i = 0;  DXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		DXGI_ADAPTER_DESC desc;
		if (SUCCEEDED(pAdapter->GetDesc(&desc))) {
			//std::wcout << L"GPU #" << i << ": " << desc.Description << std::endl;

			// 检查厂商 ID
			switch (desc.VendorId) {
			case 0x10DE: // NVIDIA
				Engine_Log("Vendor: NVIDIA");
				return NVIDIA;
				break;
			case 0x1002: // AMD
				Engine_Log("Vendor: AMD");
				return AMD;
				break;
			case 0x8086: // Intel
				Engine_Log("Vendor: INTEL");
				return INTEL;
				break;
			default:
				Engine_Log("Vendor: UNKNOWN");
				return UNKNOWN;
			}
		}
		pAdapter->Release();
	}

	return UNKNOWN;
}

UINT CDirectXRenderingEngine::GetDXGISampleCount() const
{
	return bMSAA4XEnabled ? 4 : 1;;
}

UINT CDirectXRenderingEngine::GetDXGISampleQuality() const
{
	return bMSAA4XEnabled ? (M4XQualityLevels - 1) : 0;
}

// 等待GPU处理完成
void CDirectXRenderingEngine::WaitGPUCommandQueueComplete()
{
	CurrentFenceIndex++;

	//向GUP设置新的隔离点 等待GPU处理玩信号
	ANALYSIS_HRESULT(CommandQueue->Signal(Fence.Get(), CurrentFenceIndex));

	if (Fence->GetCompletedValue() < CurrentFenceIndex)
	{
		//创建或打开一个事件内核对象,并返回该内核对象的句柄.
		//SECURITY_ATTRIBUTES
		//CREATE_EVENT_INITIAL_SET  0x00000002
		//CREATE_EVENT_MANUAL_RESET 0x00000001
		//ResetEvents
		HANDLE EventEX = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);

		//GPU完成后会通知我们的Handle
		ANALYSIS_HRESULT(Fence->SetEventOnCompletion(CurrentFenceIndex, EventEX));

		//等待GPU,阻塞主线程
		WaitForSingleObject(EventEX, INFINITE);
		CloseHandle(EventEX);
	}
}

bool CDirectXRenderingEngine::InitDirect3D()
{
	//Debug
	ComPtr<ID3D12Debug> D3D12Debug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug))))
	{
		D3D12Debug->EnableDebugLayer();
	}

	////////////////////////////////////////////////////////////////////////////////////////
		//HRESULT
		//S_OK				0x00000000
		//E_UNEXPECTED		0x8000FFFF 意外的失败
		//E_NOTIMPL			0x80004001 未实现
		//E_OUTOFMEMORY		0x8007000E 未能分配所需的内存
		//E_INVALIDARG		0x80070057 一个或多个参数无效 
		//E_NOINTERFACE		0x80004002 不支持此接口
		//E_POINTER			0x80004003 无效指针
		//E_HANDLE			0x80070006 无效句柄
		//E_ABORT			0x80004004 操作终止
		//E_FAIL			0x80004005 错误
		//E_ACCESSDENIED	0x80070005 一般的访问被拒绝错误
	ANALYSIS_HRESULT(CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory)));

	/*
	D3D_FEATURE_LEVEL_9_1  目标功能级别支持Direct3D 9.1包含 shader model 2.
	D3D_FEATURE_LEVEL_9_2  目标功能级别支持Direct3D 9.2包含 shader model 2.
	D3D_FEATURE_LEVEL_9_3  目标功能级别支持Direct3D 9.3包含 shader model 3.
	D3D_FEATURE_LEVEL_10_0 目标功能级别支持Direct3D 10.0包含 shader model 4.
	D3D_FEATURE_LEVEL_10_1 目标功能级别支持Direct3D 10.1包含 shader model 4.
	D3D_FEATURE_LEVEL_11_0 目标功能级别支持Direct3D 11.0包含 shader model 5.
	*/

	HRESULT D3dDeviceResult = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3dDevice));
	if (FAILED(D3dDeviceResult))
	{
		//warp
		ComPtr<IDXGIAdapter> WARPAdapter;
		ANALYSIS_HRESULT(DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&WARPAdapter)));
		ANALYSIS_HRESULT(D3D12CreateDevice(WARPAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3dDevice)));
	}

	//创建Fence对象 为了CPU和GPU同步作准备
	//D3D12_FENCE_FLAG_NONE 
	//D3D11_FENCE_FLAG_SHARED
	//D3D11_FENCE_FLAG_SHARED_CROSS_ADAPTER
	/*
	Fence->SetEventOnCompletion
	执行命令
	提交呈现
	Queue->Signal
	wait
	*/
	ANALYSIS_HRESULT(D3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

	//初始化命令对象
////////////////////////////////////////////////////////////////////////////////////////
	//INT Priority 
	//D3D12_COMMAND_QUEUE_PRIORITY
	//D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
	//D3D12_COMMAND_QUEUE_PRIORITY_HIGH
	//NodeMask 指示该命令队列应在哪个GPU节点上执行
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;//直接
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	ANALYSIS_HRESULT(D3dDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));

	//	ID3D12CommandAllocator Allocator();
	ANALYSIS_HRESULT(D3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CommandAllocator.GetAddressOf())));

	ANALYSIS_HRESULT(D3dDevice->CreateCommandList(
		0, //默认单个Gpu 
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,//直接类型
		CommandAllocator.Get(),//将Commandlist关联到Allocator
		NULL,//ID3D12PipelineState
		IID_PPV_ARGS(GraphicsCommandList.GetAddressOf())));

	ANALYSIS_HRESULT(GraphicsCommandList->Close());

	//多重采样
////////////////////////////////////////////////////////////////////
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS QualityLevels;
	QualityLevels.Format = BackBufferFormat;
	QualityLevels.SampleCount = 4;
	QualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS::D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	QualityLevels.NumQualityLevels = 0;

	ANALYSIS_HRESULT(D3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&QualityLevels,
		sizeof(QualityLevels)));

	M4XQualityLevels = QualityLevels.NumQualityLevels;

	// 交换链
////////////////////////////////////////////////////////////////////
	SwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferDesc.Width = FEngineRenderConfig::GetRenderConfig()->ScrrenWidth;
	SwapChainDesc.BufferDesc.Height = FEngineRenderConfig::GetRenderConfig()->ScrrenHight;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = FEngineRenderConfig::GetRenderConfig()->RefreshRate;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferCount = FEngineRenderConfig::GetRenderConfig()->SwapChainCount;

	// DXGI_USAGE_BACK_BUFFER //
	// DXGI_USAGE_READ_ONLY 
	// DXGI_USAGE_SHADER_INPUT
	// DXGI_USAGE_SHARED
	// DXGI_USAGE_UNORDERED_ACCESS
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//使用表面或资源作为输出渲染目标。
	SwapChainDesc.OutputWindow = MainWindowsHandle;//指定windows句柄
	SwapChainDesc.Windowed = true;//以窗口运行
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//IDXGISwapChain::ResizeTarget
	SwapChainDesc.BufferDesc.Format = BackBufferFormat;//纹理格式

	// 多重采样设置
	SwapChainDesc.SampleDesc.Count = GetDXGISampleCount();
	SwapChainDesc.SampleDesc.Quality = GetDXGISampleQuality();

	ANALYSIS_HRESULT(DXGIFactory->CreateSwapChain(
		CommandQueue.Get(),
		&SwapChainDesc, SwapChain.GetAddressOf()));

	// 资源描述符
	////////////////////////////////////////////////////////////////////
	// D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV	//CBV常量缓冲区视图 SRV着色器资源视图 UAV无序访问视图
	// D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER		//采样器视图
	// D3D12_DESCRIPTOR_HEAP_TYPE_RTV			//渲染目标视图资源
	// D3D12_DESCRIPTOR_HEAP_TYPE_DSV			//深度/模板的视图资源
	// RTV
	
	
	//************ ！！！每次添加新RTV时需检查此处！！！****************
	D3D12_DESCRIPTOR_HEAP_DESC RTVDescriptorHeapDesc;
	RTVDescriptorHeapDesc.NumDescriptors =
		FEngineRenderConfig::GetRenderConfig()->SwapChainCount + //交换链
		6 +			//反射CubeMap RTV
		6+          //点光源阴影cubemap  RTV
		1+	        //屏幕法线		
		1;	        //环境	


	RTVDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTVDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	RTVDescriptorHeapDesc.NodeMask = 0;
	ANALYSIS_HRESULT(D3dDevice->CreateDescriptorHeap(
		&RTVDescriptorHeapDesc,
		IID_PPV_ARGS(RTVHeap.GetAddressOf())));

	//************ !!!!! 每次添加新DSV时需检查此处 !!!!! ****************
	// 创建DSV
	D3D12_DESCRIPTOR_HEAP_DESC DSVDescriptorHeapDesc;
	DSVDescriptorHeapDesc.NumDescriptors =
		1	+			//main视口深度
		1	+			//CubeMap深度 反射
		1	+			//平行光 聚光灯 阴影深度
		1;				//点光源 CubeMapShadow深度

	DSVDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSVDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DSVDescriptorHeapDesc.NodeMask = 0;
	ANALYSIS_HRESULT(D3dDevice->CreateDescriptorHeap(
		&DSVDescriptorHeapDesc,
		IID_PPV_ARGS(DSVHeap.GetAddressOf())));

	return false;
}

void CDirectXRenderingEngine::PostInitDirect3D()
{
	// 同步
	WaitGPUCommandQueueComplete();

	ANALYSIS_HRESULT(GraphicsCommandList->Reset(CommandAllocator.Get(), NULL));

	for (int i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
	{
		SwapChainBuffer[i].Reset();
	}
	DepthStencilBuffer.Reset();

	// 自适应屏幕变大
	SwapChain->ResizeBuffers(
		FEngineRenderConfig::GetRenderConfig()->SwapChainCount,
		FEngineRenderConfig::GetRenderConfig()->ScrrenWidth,
		FEngineRenderConfig::GetRenderConfig()->ScrrenHight,
		BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	// 拿到描述size
	RTVDescriptorSize = D3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE HeapHandle(RTVHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
	{
		SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer[i]));
		D3dDevice->CreateRenderTargetView(SwapChainBuffer[i].Get(), nullptr, HeapHandle);
		HeapHandle.Offset(1, RTVDescriptorSize);
	}

	D3D12_RESOURCE_DESC ResourceDesc;
	ResourceDesc.Width = FEngineRenderConfig::GetRenderConfig()->ScrrenWidth;
	ResourceDesc.Height = FEngineRenderConfig::GetRenderConfig()->ScrrenHight;
	ResourceDesc.Alignment = 0;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	ResourceDesc.SampleDesc.Count = bMSAA4XEnabled ? 4 : 1;
	ResourceDesc.SampleDesc.Quality = bMSAA4XEnabled ? (M4XQualityLevels - 1) : 0;
	ResourceDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;


	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.DepthStencil.Depth = 1.f;
	ClearValue.DepthStencil.Stencil = 0;
	ClearValue.Format = DepthStencilFormat;

	CD3DX12_HEAP_PROPERTIES Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3dDevice->CreateCommittedResource(
		&Properties,
		D3D12_HEAP_FLAG_NONE, &ResourceDesc,
		D3D12_RESOURCE_STATE_COMMON, &ClearValue,
		IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf()));

	D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	DSVDesc.Format = DepthStencilFormat;
	DSVDesc.Texture2D.MipSlice = 0;
	DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
	D3dDevice->CreateDepthStencilView(DepthStencilBuffer.Get(), &DSVDesc, DSVHeap->GetCPUDescriptorHandleForHeapStart());

	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);

	GraphicsCommandList->ResourceBarrier(1, &Barrier);

	GraphicsCommandList->Close();

	ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);


	WaitGPUCommandQueueComplete();
}

#endif