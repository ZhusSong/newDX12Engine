﻿#pragma once
// 2025.4.李
#include "../../Core/Engine.h"
#include "../../Math/EngineMath.h"

struct FViewportTransformation
{
	FViewportTransformation();

	XMFLOAT4 ViewportPosition;
	XMFLOAT4X4 ViewProjectionMatrix;
};
