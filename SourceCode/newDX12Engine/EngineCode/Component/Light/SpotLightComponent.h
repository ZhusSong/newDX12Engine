// 聚光灯组件
// //  スポットライトコンポーネント
#pragma once
#include "Core/RangeLightComponent.h"
#include "SpotLightComponent.CodeReflection.h"

class CSpotLightComponent :public CRangeLightComponent
{
	CODEREFLECTION()
public:
	CSpotLightComponent();

	float GetConicalInnerCorner() const { return ConicalInnerCorner; }
	float GetConicalOuterCorner() const { return ConicalOuterCorner; }

	// 设置角度限制
	// 角度制限を設定する
	void SetConicalInnerCorner(float InConicalInnerCorner);
	void SetConicalOuterCorner(float InConicalOuterCorner);
protected:
	float ConicalInnerCorner; // spot 弧度   // スポットライト内角（ラジアン）
	float ConicalOuterCorner; // spot 弧度   // スポットライト外角（ラジアン）
};
