#include "TorusMesh.h"
#include "Core/MeshType.h"
#include "../Mesh/Core/MeshManager.h"
#include "../Core/Construction/MacroConstruction.h"
#include "../Component/Mesh/TorusMeshComponent.h"

GTorusMesh::GTorusMesh()
{
	BUILD_OBJECT_PARAMETERS(, this);
	SetMeshComponent(ConstructionObject<CTorusMeshComponent>(Param));
}

void GTorusMesh::Init()
{

}

void GTorusMesh::Draw(float DeltaTime)
{

}


void GTorusMesh::CreateMesh(float InRadius, float InSectionRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
	CREATE_RENDER_DATA(CTorusMeshComponent, InRadius, InSectionRadius, InAxialSubdivision, InHeightSubdivision);
}
