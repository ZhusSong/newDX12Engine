#pragma once
#include "../Core/CoreObject/CoreMinimalObject.h"

class CLightComponent;
class CLightManager :public CCoreMinimalObject
{
	friend struct FGeometryMap;
public:
	void AddLight(CLightComponent* InLightStance);

	const vector<CLightComponent*>& GetLights() const { return Lights; }

protected:
	vector<CLightComponent*> Lights;
};