#include "SpotLight.h"
#include "../../Component/Light/SpotLightComponent.h"

float index_test = 0.f;

GSpotLight::GSpotLight()
	:Super()
{
	SetLightComponent(CreateObject<CSpotLightComponent>(new CSpotLightComponent()));
	index_test = 0.f;
}

void GSpotLight::Tick(float DeltaTime)
{
	index_test += DeltaTime;

	float I = 1.f;
	float O = 40.f;

	SetConicalInnerCorner(I * fabsf(cos(index_test)) + 10.f);
	SetConicalOuterCorner(O * fabsf(cos(index_test)) + 30.f);

	fvector_3d v3 = GetRotation();
	v3.x += DeltaTime * 60.f;
	//v3.y += DeltaTime * 40.f;
	////v3.z += DeltaTime * 100.f;

	SetRotation(v3);
}

float GSpotLight::GetConicalInnerCorner() const
{
	if (CSpotLightComponent* InSpotLightComponent = dynamic_cast<CSpotLightComponent*>(const_cast<CLightComponent*>(GetLightComponent())))
	{
		InSpotLightComponent->GetConicalInnerCorner();
	}

	return 0.0f;
}

float GSpotLight::GetConicalOuterCorner() const
{
	if (CSpotLightComponent* InSpotLightComponent = dynamic_cast<CSpotLightComponent*>(const_cast<CLightComponent*>(GetLightComponent())))
	{
		InSpotLightComponent->GetConicalOuterCorner();
	}

	return 0.0f;
}

void GSpotLight::SetConicalInnerCorner(float InConicalInnerCorner)
{
	if (CSpotLightComponent* InSpotLightComponent = dynamic_cast<CSpotLightComponent*>(const_cast<CLightComponent*>(GetLightComponent())))
	{
		InSpotLightComponent->SetConicalInnerCorner(InConicalInnerCorner);
	}
}

void GSpotLight::SetConicalOuterCorner(float InConicalOuterCorner)
{
	if (CSpotLightComponent* InSpotLightComponent = dynamic_cast<CSpotLightComponent*>(const_cast<CLightComponent*>(GetLightComponent())))
	{
		InSpotLightComponent->SetConicalOuterCorner(InConicalOuterCorner);
	}
}
