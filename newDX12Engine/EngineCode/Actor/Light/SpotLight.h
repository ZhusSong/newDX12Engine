// 25.5.29 李
#pragma once
#include "Core/RangeLight.h"

//G -> Game
class GSpotLight :public GRangeLight
{
	typedef GRangeLight Super;

public:
	GSpotLight();

	virtual void Tick(float DeltaTime);

	float GetConicalInnerCorner() const;
	float GetConicalOuterCorner() const;

	void SetConicalInnerCorner(float InConicalInnerCorner);
	void SetConicalOuterCorner(float InConicalOuterCorner);

};