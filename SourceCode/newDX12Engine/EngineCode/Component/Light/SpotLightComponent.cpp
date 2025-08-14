// 25.5.29 李
//  点光源组件
#include "SpotLightComponent.h"
#include "../../Mesh/Core/MeshManager.h"
#include "../../Component/Mesh/Core/MeshComponent.h"
#include "../../Mesh/Core/Material/Material.h"

CSpotLightComponent::CSpotLightComponent()
	:Super()
	, ConicalInnerCorner(30.f)
	, ConicalOuterCorner(60.f)
{

	string MeshPath = FEnginePathHelper::GetEngineAssetPath() + "/SpotMesh.obj";

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

	LightType = ELightType::SpotLight;
}

void CSpotLightComponent::SetConicalInnerCorner(float InConicalInnerCorner)
{
	if (ConicalOuterCorner < InConicalInnerCorner)
	{
		ConicalOuterCorner = InConicalInnerCorner;
		ConicalInnerCorner = InConicalInnerCorner;
	}
	else if (ConicalOuterCorner > InConicalInnerCorner)
	{
		ConicalInnerCorner = InConicalInnerCorner;
	}
}

void CSpotLightComponent::SetConicalOuterCorner(float InConicalOuterCorner)
{
	if (ConicalInnerCorner > InConicalOuterCorner)
	{
		ConicalOuterCorner = InConicalOuterCorner;
		ConicalInnerCorner = InConicalOuterCorner;
	}
	else if (ConicalInnerCorner < InConicalOuterCorner)
	{
		ConicalOuterCorner = InConicalOuterCorner;
	}
}
