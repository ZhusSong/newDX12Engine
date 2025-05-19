// 平行光组件
#include "ParallelLightComponent.h"
#include "../../Mesh/Core/MeshManager.h"
#include "../../Component/Mesh/Core/MeshComponent.h"
#include "../../Mesh/Core/Material/Material.h"

CParallelLightComponent::CParallelLightComponent()
{
	// 读取模型资源
	string MeshPath = "../newDX12Engine/Asset/SunMesh.obj";

	SetLightMesh(GetMeshManager()->CreateMeshComponent(MeshPath)); if (GetLightMesh())
	{
		if (CMaterial* InMaterial = (*GetLightMesh()->GetMaterials())[0])
		{
			InMaterial->SetMaterialType(EMaterialType::BaseColor);
			//InMaterial->SetMaterialDisplayStatus(EMaterialDisplayStatusType::TriangleDisplay);
			InMaterial->SetBaseColor(fvector_4d(1.0f, 0.7f, 1.0f, 1.0f));
		}
	}

}
