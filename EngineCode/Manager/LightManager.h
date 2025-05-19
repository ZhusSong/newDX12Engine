#pragma once
#include "../Core/CoreObject/CoreMinimalObject.h"

class CLightComponent;
class CLightManager :public CCoreMinimalObject
{
	friend struct FGeometryMap;
public:
	void AddLight(CLightComponent* InLightStance);

protected:
	vector<CLightComponent*> Lights;
};