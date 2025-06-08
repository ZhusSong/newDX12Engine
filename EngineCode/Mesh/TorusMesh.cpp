#include "TorusMesh.h"
#include "Core/MeshType.h"
#include "../Mesh/Core/MeshManager.h"

void GTorusMesh::Init()
{

}

void GTorusMesh::Draw(float DeltaTime)
{

}

void GTorusMesh::CreateMesh(float InRadius, float InSectionRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
	SetMeshComponent(GetMeshManager()->CreateTorusMeshComponent(InRadius, InSectionRadius, InAxialSubdivision, InHeightSubdivision));
}

