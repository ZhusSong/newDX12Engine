#pragma once
#include "../Core/ActorObject.h"
#include "Fog.CodeReflection.h"

class CFogComponent;

class GFog :public GActorObject
{

	CODEREFLECTION()

	CVARIABLE()
	CFogComponent* FogComponent;

public:
	virtual void Tick(float DeltaTime);

public:
	GFog();

	void SetFogColor(const fvector_color& InNewColor);
	void SetFogStart(float InNewFogStart);
	void SetFogRange(float InNewFogRange);
	void SetFogHeight(float InNewFogHeight);
	void SetFogTransparentCoefficient(float InNewFogTransparentCoefficient);
};