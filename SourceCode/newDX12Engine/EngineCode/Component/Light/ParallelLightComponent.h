// 平行光组件
// コリメート光のコンポーネント
#pragma once
#include "Core/LightComponent.h"
#include "ParallelLightComponent.CodeReflection.h"

class CParallelLightComponent :public CLightComponent
{
	CODEREFLECTION()
public:
	CParallelLightComponent();
};
