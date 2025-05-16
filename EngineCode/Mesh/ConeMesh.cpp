#include "ConeMesh.h"
#include "Core/MeshType.h"
#include "../Mesh/Core/MeshManager.h"

void GConeMesh::Init()
{
	Super::Init();

}



void GConeMesh::Draw(float DeltaTime)
{
	Super::Draw(DeltaTime);


}

void GConeMesh::CreateMesh(
	float InRadius,
	float InHeight,
	uint32_t InAxialSubdivision,
	uint32_t InHeightSubdivision)
{
	SetMeshComponent(GetMeshManager()->CreateConeMeshComponent(InRadius, InHeight, InAxialSubdivision, InHeightSubdivision));
}