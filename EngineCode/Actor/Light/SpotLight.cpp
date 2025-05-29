#include "SpotLight.h"
#include "../../Component/Light/SpotLightComponent.h"

GSpotLight::GSpotLight()
{
	SetLightComponent(CreateObject<CSpotLightComponent>(new CSpotLightComponent()));
}

void GSpotLight::Tick(float DeltaTime)
{
	//fvector_3d v3 = GetRotation();

	////v3.x += DeltaTime * 40.f;
	////v3.y += DeltaTime * 40.f;
	//////v3.z += DeltaTime * 100.f;

	//SetRotation(v3);
}

float GSpotLight::GetStartAttenuation() const
{
	if (const CSpotLightComponent* InSpotLightComponent = dynamic_cast<const CSpotLightComponent*>(GetLightComponent()))
	{
		return InSpotLightComponent->GetStartAttenuation();
	}

	return 0.f;
}

float GSpotLight::GetEndAttenuation() const
{
	if (const CSpotLightComponent* InSpotLightComponent = dynamic_cast<const CSpotLightComponent*>(GetLightComponent()))
	{
		return InSpotLightComponent->GetEndAttenuation();
	}

	return 0.f;
}

void GSpotLight::SetStartAttenuation(float InNewStartAttenuation)
{
	if (CSpotLightComponent* InSpotLightComponent = dynamic_cast<CSpotLightComponent*>(const_cast<CLightComponent*>(GetLightComponent())))
	{
		InSpotLightComponent->SetStartAttenuation(InNewStartAttenuation);
	}
}

void GSpotLight::SetEndAttenuation(float InNewEndAttenuation)
{
	if (CSpotLightComponent* InSpotLightComponent = dynamic_cast<CSpotLightComponent*>(const_cast<CLightComponent*>(GetLightComponent())))
	{
		InSpotLightComponent->SetEndAttenuation(InNewEndAttenuation);
	}
}
