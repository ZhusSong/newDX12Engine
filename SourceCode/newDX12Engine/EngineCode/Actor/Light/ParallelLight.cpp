#include "ParallelLight.h"
#include "../../Core/Construction/MacroConstruction.h"

GParallelLight::GParallelLight()
{
	BUILD_OBJECT_PARAMETERS(Type, this);
	SetLightComponent(CreateObject<CParallelLightComponent>(ParamType, new CParallelLightComponent()));
}
void GParallelLight::Tick(float DeltaTime)
{
	fvector_3d v3 = fvector_3d();

	//v3.x -= DeltaTime * 30.f;
	v3.y -= DeltaTime * 30.f;
	//v3.z += DeltaTime * 100.f;

	//叠加
	SetRotation(v3);
}