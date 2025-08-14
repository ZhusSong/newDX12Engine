// 灯光组件
#include "LightComponent.h"
#include "../../../Manager/LightManager.h"
#include "../../Mesh/Core/MeshComponent.h"


CLightComponent::CLightComponent()
	:LightIntensity(1.f, 1.f, 1.f)
{
	GetLightManager()->AddLight(this);
}

CLightComponent::~CLightComponent()
{
}
void CLightComponent::SetLightIntensity(const fvector_3d& InNewLightIntensity)
{
	LightIntensity = InNewLightIntensity;
}

void CLightComponent::SetPosition(const XMFLOAT3& InNewPosition)
{
	Super::SetPosition(InNewPosition);

	if (LightMesh)
	{
		LightMesh->SetPosition(InNewPosition);
	}
}

void CLightComponent::SetRotation(const fvector_3d& InNewRotation)
{
	Super::SetRotation(InNewRotation);

	// 取反，保证灯光方向与shader中算法一致
	if (LightMesh)
	{
		LightMesh->SetRotation(InNewRotation * (-1.f));
	}
}

void CLightComponent::SetScale(const fvector_3d& InNewScale)
{
	Super::SetScale(InNewScale);

	if (LightMesh)
	{
		LightMesh->SetScale(InNewScale);
	}
}

void CLightComponent::SetForwardVector(const XMFLOAT3& InForwardVector)
{
	Super::SetForwardVector(InForwardVector);

	if (LightMesh)
	{
		LightMesh->SetForwardVector(InForwardVector);
	}
}

void CLightComponent::SetRightVector(const XMFLOAT3& InRightVector)
{
	Super::SetRightVector(InRightVector);

	if (LightMesh)
	{
		LightMesh->SetRightVector(InRightVector);
	}
}

void CLightComponent::SetUPVector(const XMFLOAT3& InUPVector)
{
	Super::SetUPVector(InUPVector);

	if (LightMesh)
	{
		LightMesh->SetUPVector(InUPVector);
	}
}

void CLightComponent::SetLightMesh(CMeshComponent* InLightMesh)
{
	LightMesh = InLightMesh;

	// 灯光不投射阴影
	LightMesh->SetCastShadow(false);
}
