#include "MeshComponent.h"
#include "../../../Mesh/Core/Material/Material.h"
#include "../../../Rendering/Core/DirectX/RenderingPipeline/Geometry/GeometryMap.h"
#include "../../../Core/Construction/MacroConstruction.h"

CMeshComponent::CMeshComponent()
{
	MeshRenderLayerType = EMeshRenderLayerType::RENDERLAYER_OPAQUE;

	BUILD_OBJECT_PARAMETERS_BY_COMPONENT(, this);
	Materials.push_back(CreateObject<CMaterial>(Param, new CMaterial()));

	bCastShadow = true;
	bPickup = true;
	bVisible = true;
}

void CMeshComponent::Init()
{
}

void CMeshComponent::BuildMesh(const FMeshRenderingData* InRenderingData)
{
}
void CMeshComponent::SetMeshRenderLayerType(EMeshRenderLayerType InRenderLayerType)
{
	MeshRenderLayerType = InRenderLayerType;
}

void CMeshComponent::GetBoundingBox(BoundingBox& OutBoundingBox)
{
	FGeometry::FindRenderingDatas(
		[&](std::shared_ptr<FRenderingData>& InRenderingData)->EFindValueType
		{
			if (InRenderingData->Mesh == this)
			{
				OutBoundingBox = InRenderingData->Bounds;

				return EFindValueType::TYPE_COMPLETE;
			}

			return EFindValueType::TYPE_IN_PROGRAM;
		});
}

BoundingBox CMeshComponent::GetBoundingBox()
{
	BoundingBox AABB;
	GetBoundingBox(AABB);

	return AABB;
}
UINT CMeshComponent::GetMaterialNum() const
{
	return Materials.size();
}

void CMeshComponent::SetMaterialCount(UINT InMaterialCount)
{
	BUILD_OBJECT_PARAMETERS_BY_COMPONENT(, this);

	while (Materials.size() < InMaterialCount)
	{
		Materials.push_back(CreateObject<CMaterial>(Param, new CMaterial()));
	}

	if (Materials.size() > InMaterialCount)
	{
		Materials.resize(InMaterialCount);
	}
}

CMaterial* CMeshComponent::GetMaterialBySlot(UINT InMaterialSlot) const
{
	if (InMaterialSlot < Materials.size())
	{
		return Materials[InMaterialSlot];
	}

	if (!Materials.empty())
	{
		return Materials[0];
	}

	return nullptr;
}

void CMeshComponent::ApplyCachedMeshMetadata(const size_t InMeshHash)
{
}


bool CMeshComponent::IsDynamicReflection() const
{
	if (Materials.size() >= 1)
	{
		return
			Materials[0]->IsDynamicReflection() &&
			MeshRenderLayerType == EMeshRenderLayerType::RENDERLAYER_OPAQUE_REFLECTOR;
	}

	return false;
}

