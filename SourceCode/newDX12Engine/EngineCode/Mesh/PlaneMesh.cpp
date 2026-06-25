#include "PlaneMesh.h"
#include "Core/MeshType.h"
#include "../Mesh/Core/MeshManager.h"
#include "../Core/Construction/MacroConstruction.h"
#include "../Component/Mesh/PlaneMeshComponent.h"

GPlaneMesh::GPlaneMesh()
{
	BUILD_OBJECT_PARAMETERS(, this);
	SetMeshComponent(ConstructionObject<CPlaneMeshComponent>(Param));
}

void GPlaneMesh::Init()
{
	Super::Init();
}

void GPlaneMesh::Draw(float DeltaTime)
{
	Super::Draw(DeltaTime);
}

void GPlaneMesh::CreateMesh(
	float InHeight,
	float InWidth,
	uint32_t InHeightSubdivide,
	uint32_t InWidthSubdivide,
	const fvector_3d& InGenerateRotation)
{
	CREATE_RENDER_DATA(CPlaneMeshComponent, InHeight, InWidth, InHeightSubdivide, InWidthSubdivide, InGenerateRotation);
}
