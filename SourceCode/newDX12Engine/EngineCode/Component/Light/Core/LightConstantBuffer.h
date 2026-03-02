// 灯光常量缓冲区
// ライト用定数バッファ
#pragma once
#include "../../../EngineMinimal.h"

struct FLight
{
	FLight();

	// 灯光强度
	// ライトの強度
	XMFLOAT3 LightIntensity;
	// 开始衰减时的值
	// 減衰開始時の値
	float StartAttenuation;

	// 灯光方向
	// ライトの方向
	XMFLOAT3 LightDirection;
	// 最终衰减的值
	// 最終減衰時の値
	float EndAttenuation;

	// 灯光位置
	// ライトの位置
	XMFLOAT3 Position;

	// 灯光种类
	// ライトの種類
	int LightType;

	// 聚光灯内角(弧度制)
	// スポットライトの内角（ラジアン）
	float ConicalInnerCorner;
	// 聚光灯外角(弧度制)
	// スポットライトの外角（ラジアン）
	float ConicalOuterCorner;

	// 后续添加
	// 今後追加予定
	float xx1;
	float xx2;

	// 灯光视角
	// ライトのビュー行列（シャドウ用）
	XMFLOAT4X4 ShadowTransform;
};
struct FLightConstantBuffer
{
	FLightConstantBuffer();

	FLight SceneLights[16];
};
