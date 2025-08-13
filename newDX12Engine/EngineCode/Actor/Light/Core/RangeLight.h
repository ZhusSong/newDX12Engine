// 25.5.29 李 范围灯光actor，基于GLight
#pragma once
#include "Light.h"

//G -> Game
class GRangeLight :public GLight
{
	typedef GLight Super;

public:
	float GetStartAttenuation() const;
	float GetEndAttenuation() const;

public:
	void SetStartAttenuation(float InNewStartAttenuation);
	void SetEndAttenuation(float InNewEndAttenuation);
};