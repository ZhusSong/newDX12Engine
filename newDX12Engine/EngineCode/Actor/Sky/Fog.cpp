#include "Fog.h"
#include "../../Component/Sky/FogComponent.h"

GFog::GFog()
{
	FCreateObjectParam Param;
	Param.Outer = this;

	FogComponent = CreateObject<CFogComponent>(Param, new CFogComponent());
}
void GFog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void GFog::SetFogColor(const fvector_color& InNewColor)
{
	FogComponent->SetFogColor(InNewColor);
}

void GFog::SetFogStart(float InNewFogStart)
{
	FogComponent->SetFogStart(InNewFogStart);
}

void GFog::SetFogRange(float InNewFogRange)
{
	FogComponent->SetFogRange(InNewFogRange);
}

void GFog::SetFogHeight(float InNewFogHeight)
{
	FogComponent->SetFogHeight(InNewFogHeight);
}

void GFog::SetFogTransparentCoefficient(float InNewFogTransparentCoefficient)
{
	FogComponent->SetFogTransparentCoefficient(InNewFogTransparentCoefficient);
}
