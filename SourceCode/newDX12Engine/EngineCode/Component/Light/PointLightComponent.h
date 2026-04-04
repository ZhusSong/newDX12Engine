// 点光源组件
//  点光源コンポーネント
#pragma once
#include "Core/RangeLightComponent.h"
#include "PointLightComponent.CodeReflection.h"

class CPointLightComponent :public CRangeLightComponent
{
	CODEREFLECTION()
public:
	CPointLightComponent();
};
