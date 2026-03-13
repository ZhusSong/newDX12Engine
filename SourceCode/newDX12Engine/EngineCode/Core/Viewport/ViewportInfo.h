#pragma once
#include "../../Core/Engine.h"

struct FViewportInfo
{
	// 定义摄像机位置,UP，Right,Look
	// カメラの位置、Up、Right、Look を定義
	XMFLOAT4 ViewPosition;

	XMFLOAT4X4 ViewMatrix;

	XMFLOAT4X4 ProjectMatrix;
};