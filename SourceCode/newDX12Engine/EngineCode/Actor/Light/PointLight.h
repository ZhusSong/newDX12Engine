#pragma once
#include "Core/RangeLight.h"

//G -> Game
class GPointLight :public GRangeLight
{
	typedef GRangeLight Super;

public:
	GPointLight();

	virtual void Tick(float DeltaTime);
	float time;
};