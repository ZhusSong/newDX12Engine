// 25.5.29 李
//  点光源组件
#include "PointLightComponent.h"
#include "../../Mesh/Core/MeshManager.h"
#include "../../Component/Mesh/Core/MeshComponent.h"
#include "../../Mesh/Core/Material/Material.h"

CPointLightComponent::CPointLightComponent()
	:Super()
{
	// 加载模型
	string MeshPath = FEnginePathHelper::GetEngineAssetPath() +  "/PointMesh.obj";
	
	//string MeshPath = "Asset/PointMesh.obj";
	FCreateObjectParam Param;
	Param.Outer = this;
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
