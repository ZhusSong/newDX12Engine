// 25.5.29 李 
// 点光源组件
#pragma once
#include "Core/LightComponent.h"

class CMeshComponent;
class CSpotLightComponent :public CLightComponent
{
	typedef CLightComponent Super;
public:
	CSpotLightComponent();

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
