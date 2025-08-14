// 25.5.29 李 
// 聚光灯组件
#pragma once
#include "Core/RangeLightComponent.h"

class CSpotLightComponent :public CRangeLightComponent
{
	typedef CRangeLightComponent Super;
public:
	CSpotLightComponent();

	float GetConicalInnerCorner() const { return ConicalInnerCorner; }
	float GetConicalOuterCorner() const { return ConicalOuterCorner; }

	// 设置角度限制
	void SetConicalInnerCorner(float InConicalInnerCorner);
	void SetConicalOuterCorner(float InConicalOuterCorner);
protected:
	float  ConicalInnerCorner;//spot 弧度
	float  ConicalOuterCorner;//spot 弧度
};
