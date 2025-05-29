#pragma once
#include "Core/Light.h"
#include "../../Component/Light/ParallelLightComponent.h"

//G -> Game
class GParallelLight :public GLight
{
	typedef GLight Super;

public:
	GParallelLight();

	virtual void Tick(float DeltaTime);
};