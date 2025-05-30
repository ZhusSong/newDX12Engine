// 范围灯光组件
// 25.5.30 李
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
