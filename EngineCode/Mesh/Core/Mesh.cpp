// 2025.4.李
#include "Mesh.h"
#include "../../Config/EngineRenderConfig.h"
#include "../../Component/TransformComponent.h"
#include "Material/Material.h"
#include "../../Component/Mesh/Core/MeshComponent.h"

GMesh::GMesh()
	:GActorObject()
{

}

void GMesh::Init()
{
}

void GMesh::PreDraw(float DeltaTime)
{

}

void GMesh::Draw(float DeltaTime)
{
}

void GMesh::PostDraw(float DeltaTime)
{

}

void GMesh::SetPosition(const XMFLOAT3& InNewPosition)
{
	Super::SetPosition(InNewPosition);

	// 通过mesh组件设置位置
	GetMeshComponent()->SetPosition(InNewPosition);
}

void GMesh::SetRotation(const fvector_3d& InRotation)
{
	Super::SetRotation(InRotation);

	// 通过mesh组件设置旋转
	GetMeshComponent()->SetRotation(InRotation);
}

void GMesh::SetScale(const fvector_3d& InNewScale)
{
	Super::SetScale(InNewScale);

	// 通过mesh组件设置大小
	GetMeshComponent()->SetScale(InNewScale);
}

UINT GMesh::GetMaterialNum() const
{
	return MeshComponent->GetMaterialNum();
}

vector<CMaterial*>* GMesh::GetMaterials()
{
	return MeshComponent->GetMaterials();
}

void GMesh::SetMeshComponent(CMeshComponent* InMeshComponent)
{
	MeshComponent = InMeshComponent;
}