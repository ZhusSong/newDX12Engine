#pragma once
#include "../../Core/Engine.h"

struct FViewportInfo
{
	//定义摄像机位置,UP，Right,Look
	XMFLOAT4 ViewPosition;

	XMFLOAT4X4 ViewMatrix;

	XMFLOAT4X4 ProjectMatrix;
};