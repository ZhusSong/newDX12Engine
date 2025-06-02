// 灯光常量缓冲区
#pragma once
#include "../../../EngineMinimal.h"

struct FLight
{
	FLight();

	// 灯光强度
	XMFLOAT3 LightIntensity;
	// 开始衰减时的值
	float StartAttenuation;

	// 灯光方向
	XMFLOAT3 LightDirection;
	// 最终衰减的值
	float EndAttenuation;

	// 灯光位置
	XMFLOAT3 Position;

	// 灯光种类
	int	   LightType;

	// 聚光灯内角(弧度制)
	float  ConicalInnerCorner;
	// 聚光灯外角(弧度制)
	float  ConicalOuterCorner;

	// 后续添加
	float xx1;
	float xx2;
};
struct FLightConstantBuffer
{
	FLightConstantBuffer();

	FLight SceneLights[16];
};
