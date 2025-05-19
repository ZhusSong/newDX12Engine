// 灯光常量缓冲区
#pragma once
#include "../../../EngineMinimal.h"

struct FLightConstantBuffer
{
	FLightConstantBuffer();

	// 灯光强度
	XMFLOAT3 LightIntensity;
	float XX = 0.f;

	// 灯光方向
	XMFLOAT3 LightDirection;
	float XX1 = 0.f;
};
