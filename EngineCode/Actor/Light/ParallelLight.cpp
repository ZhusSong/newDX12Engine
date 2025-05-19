// 平行光actor
#include "ParallelLight.h"

GParallelLight::GParallelLight()
{
	ParallelLightComponent = CreateObject<CParallelLightComponent>(new CParallelLightComponent());
}

// 每帧旋转
void GParallelLight::Tick(float DeltaTime)
{
	fvector_3d v3 = GetRotation();

	v3.x += DeltaTime * 40.f;
	//v3.y += DeltaTime * 40.f;
	//v3.z += DeltaTime * 100.f;

	SetRotation(v3);
}

void GParallelLight::SetPosition(const XMFLOAT3& InNewPosition)
{
	Super::SetPosition(InNewPosition);

	ParallelLightComponent->SetPosition(InNewPosition);
}

void GParallelLight::SetRotation(const fvector_3d& InRotation)
{
	Super::SetRotation(InRotation);

	ParallelLightComponent->SetRotation(InRotation);
}

void GParallelLight::SetScale(const fvector_3d& InNewScale)
{
	Super::SetScale(InNewScale);

	ParallelLightComponent->SetScale(InNewScale);
}
