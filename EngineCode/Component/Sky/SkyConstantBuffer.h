#pragma once
//25.6.16 李
#pragma once
#include "../../EngineMinimal.h"

struct FFogConstantBuffer
{
	FFogConstantBuffer();

	XMFLOAT3 FogColor;
	float FogStart;

	float FogRange;
	float FogHeight;
	float FogTransparentCoefficient;
	float xx2;
};