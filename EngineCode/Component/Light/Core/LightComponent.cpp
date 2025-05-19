// 灯光组件
#include "LightComponent.h"
#include "../../../Manager/LightManager.h"
#include "../../Mesh/Core/MeshComponent.h"


CLightComponent::CLightComponent()
{
	GetLightManager()->AddLight(this);
}

CLightComponent::~CLightComponent()
{
	//ﾒﾔｺﾙﾗ・
}

void CLightComponent::SetPosition(const XMFLOAT3& InNewPosition)
{
	Super::SetPosition(InNewPosition);

	LightMesh->SetPosition(InNewPosition);
}

void CLightComponent::SetRotation(const fvector_3d& InNewRotation)
{
	Super::SetRotation(InNewRotation);

	// 取反，保证灯光方向与shader中算法一致
	LightMesh->SetRotation(InNewRotation * (-1.f));
}

void CLightComponent::SetScale(const fvector_3d& InNewScale)
{
	Super::SetScale(InNewScale);

	LightMesh->SetScale(InNewScale);
}

void CLightComponent::SetForwardVector(const XMFLOAT3& InForwardVector)
{
	Super::SetForwardVector(InForwardVector);

	LightMesh->SetForwardVector(InForwardVector);
}

void CLightComponent::SetRightVector(const XMFLOAT3& InRightVector)
{
	Super::SetRightVector(InRightVector);

	LightMesh->SetRightVector(InRightVector);
}

void CLightComponent::SetUPVector(const XMFLOAT3& InUPVector)
{
	Super::SetUPVector(InUPVector);

	LightMesh->SetUPVector(InUPVector);
}

void CLightComponent::SetLightMesh(CMeshComponent* InLightMesh)
{
	LightMesh = InLightMesh;
}
