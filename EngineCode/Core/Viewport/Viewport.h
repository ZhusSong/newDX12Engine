#pragma once
#include "../../Core/Engine.h"

class FViewport
{
public:
	FViewport();

	void ViewportInit();

	// 视口矩阵
	XMFLOAT4X4 ViewMatrix;

	// 物体矩阵
	XMFLOAT4X4 ProjectMatrix;
};