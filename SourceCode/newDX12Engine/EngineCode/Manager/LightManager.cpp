#include "LightManager.h"

void CLightManager::AddLight(CLightComponent* InLightStance)
{
	Lights.push_back(InLightStance);
}
