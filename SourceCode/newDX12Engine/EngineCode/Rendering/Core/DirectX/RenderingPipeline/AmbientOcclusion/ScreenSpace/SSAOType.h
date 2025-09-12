#pragma once
#include "../../../../../../EngineMinimal.h"

// SSAO常量缓冲区数据
struct FSSAOViewportTransformation
{
	FSSAOViewportTransformation();

	XMFLOAT4X4 ProjectionMatrix;
	XMFLOAT4X4 InversiveProjectionMatrix;
	XMFLOAT4X4 TexProjectionMatrix;
};
