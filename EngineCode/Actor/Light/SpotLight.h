// 25.5.29 李
#pragma once
#include "Core/Light.h"

//G -> Game
class GSpotLight :public GLight
{
	typedef GSpotLight Super;

public:
	GSpotLight();

	virtual void Tick(float DeltaTime);

public:
	float GetStartAttenuation() const;
	float GetEndAttenuation() const;

public:
	void SetStartAttenuation(float InNewStartAttenuation);
	void SetEndAttenuation(float InNewEndAttenuation);
};