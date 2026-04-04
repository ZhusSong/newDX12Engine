#pragma once
#include "Core/Light.h"
#include "../../Component/Light/ParallelLightComponent.h"
#include "ParallelLight.CodeReflection.h"

//G -> Game
class GParallelLight :public GLight
{
	CODEREFLECTION()

public:
	GParallelLight();

	virtual void Tick(float DeltaTime);
};