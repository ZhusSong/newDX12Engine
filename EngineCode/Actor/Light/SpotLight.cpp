#include "SpotLight.h"
#include "../../Component/Light/SpotLightComponent.h"

GSpotLight::GSpotLight()
	:Super()
{
	SetLightComponent(CreateObject<CSpotLightComponent>(new CSpotLightComponent()));
}

void GSpotLight::Tick(float DeltaTime)
{
	fvector_3d v3 = GetRotation();

	v3.x += DeltaTime * 40.f;
	//v3.y += DeltaTime * 40.f;
	//////v3.z += DeltaTime * 100.f;

	SetRotation(v3);
}