#pragma once
#include "Core/RangeLight.h"
#include "PointLight.CodeReflection.h"

//G -> Game
class GPointLight :public GRangeLight
{
	CODEREFLECTION()

public:
	GPointLight();

	virtual void Tick(float DeltaTime);
	float time;
};