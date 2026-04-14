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
void GMesh::SetPickup(bool bNewPickup)
{
	GetMeshComponent()->SetPickup(bNewPickup);
}
void GMesh::SetPosition(const XMFLOAT3& InNewPosition)
{
	Super::SetPosition(InNewPosition);

	// 通过mesh组件设置位置
	// Meshコンポーネントを通して位置を設定
	GetMeshComponent()->SetPosition(InNewPosition);
}

void GMesh::SetRotation(const fvector_3d& InRotation)
{
	Super::SetRotation(InRotation);

	// 通过mesh组件设置旋转
	// Meshコンポーネントを通して回転を設定
	GetMeshComponent()->SetRotation(InRotation);
}
void GMesh::SetRotation(const fvector_3d& InRotation, bool bAbsoluteRotation)
{
	if (bAbsoluteRotation)
	{
		Super::SetRotation(InRotation, bAbsoluteRotation);
		GetMeshComponent()->SetRotation(InRotation, bAbsoluteRotation);
	}

}

void GMesh::SetScale(const fvector_3d& InNewScale)
{
	Super::SetScale(InNewScale);

	// 通过mesh组件设置大小
	// Meshコンポーネントを通してスケールを設定
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

void GMesh::SetMeshRenderLayerType(EMeshRenderLayerType InRenderLayerType)
{
	MeshComponent->SetMeshRenderLayerType(InRenderLayerType);
}

void GMesh::SetCastShadow(bool bNewCastShadow)
{
	if (MeshComponent)
	{
		MeshComponent->SetCastShadow(bNewCastShadow);
	}
}

bool GMesh::IsCastShadow() const
{
	return MeshComponent->IsCastShadow();
}