#pragma once
#include "Core/RangeLight.h"
#include "SpotLight.CodeReflection.h"

//G -> Game
class GSpotLight :public GRangeLight
{
	CODEREFLECTION()

public:
	GSpotLight();

	virtual void Tick(float DeltaTime);

	float GetConicalInnerCorner() const;
	float GetConicalOuterCorner() const;

	void SetConicalInnerCorner(float InConicalInnerCorner);
	void SetConicalOuterCorner(float InConicalOuterCorner);

};