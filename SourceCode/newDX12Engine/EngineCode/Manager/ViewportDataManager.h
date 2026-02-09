#pragma once

#include "../EngineMinimal.h"

class FViewportDataManager
{
	friend class GClientViewport;

public:
	FViewportDataManager();

	void ResetSize(int InWidth, int InHeight);

protected:
	int Width;
	int Height;

	float YFOV;
	float Aspect;
	float ZNear;
	float ZFar;
};