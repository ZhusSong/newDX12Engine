#pragma once
#include "Light.h"
#include "RangeLight.CodeReflection.h"

//G -> Game
class GRangeLight :public GLight
{
	CODEREFLECTION()

public:
	float GetStartAttenuation() const;
	float GetEndAttenuation() const;

public:
	void SetStartAttenuation(float InNewStartAttenuation);
	void SetEndAttenuation(float InNewEndAttenuation);
};