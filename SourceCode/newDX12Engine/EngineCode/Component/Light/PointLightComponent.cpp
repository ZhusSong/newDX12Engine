//  点光源组件
//  点光源コンポーネント
#include "PointLightComponent.h"
#include "../../Mesh/Core/MeshManager.h"
#include "../../Component/Mesh/Core/MeshComponent.h"
#include "../../Mesh/Core/Material/Material.h"
#include "../../Core/Construction/MacroConstruction.h"

CPointLightComponent::CPointLightComponent()
	:Super()
{	
	// 加载模型
	// モデルを読み込む
	string MeshPath = FEnginePathHelper::GetEngineAssetPath() +  "/PointMesh.obj";
	
	//string MeshPath = "Asset/PointMesh.obj";
	BUILD_OBJECT_PARAMETERS_BY_COMPONENT(, this);
	SetLightMesh(GetMeshManager()->CreateMeshComponent(Param, MeshPath));



	if (GetLightMesh())
	{
		if (CMaterial* InMaterial = (*GetLightMesh()->GetMaterials())[0])
		{
			InMaterial->SetMaterialType(EMaterialType::BaseColor);
			InMaterial->SetMaterialDisplayStatus(EMaterialDisplayStatusType::TriangleDisplay);
			InMaterial->SetBaseColor(fvector_4d(1.0f, 0.7f, 1.0f, 1.0f));
		}
	}

	LightType = ELightType::PointLight;
}
