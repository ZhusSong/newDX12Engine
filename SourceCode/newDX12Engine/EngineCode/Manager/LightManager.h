#pragma once
#include "CoreObject/CoreMinimalObject.h"
#include "LightManager.CodeReflection.h"

class CLightComponent;
class CLightManager :public CCoreMinimalObject
{
	CODEREFLECTION()
	friend struct FGeometryMap;
public:
	void AddLight(CLightComponent* InLightStance);

	const vector<CLightComponent*>& GetLights() const { return Lights; }

protected:
	vector<CLightComponent*> Lights;
};