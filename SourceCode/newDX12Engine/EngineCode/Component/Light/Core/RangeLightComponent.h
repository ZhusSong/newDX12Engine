
#pragma once
#include "LightComponent.h"

class CRangeLightComponent :public CLightComponent
{
	typedef CLightComponent Super;
public:
	CRangeLightComponent();

public:
	void SetStartAttenuation(float InNewStartAttenuation) { StartAttenuation = InNewStartAttenuation; }
	void SetEndAttenuation(float InNewEndAttenuation) { EndAttenuation = InNewEndAttenuation; }
public:
	float GetStartAttenuation() const { return StartAttenuation; }
	float GetEndAttenuation() const { return EndAttenuation; }

protected:
	float StartAttenuation;
	float EndAttenuation;
};
