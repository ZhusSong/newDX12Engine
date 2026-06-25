#pragma once
#include "../../../../../EngineMinimal.h"

struct FPlanarReflectionConstantBuffer
{
	FPlanarReflectionConstantBuffer();

	XMFLOAT4X4 ReflectionTexViewProjection;
	XMFLOAT4 Settings;
	XMFLOAT4 MirrorPlaneNormal;
	XMFLOAT4 MirrorPlanePosition;
};
