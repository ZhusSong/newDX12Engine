#pragma once
#include "../../../../../../EngineMinimal.h"

// SSAO常量缓冲区数据
struct FSSAOViewportTransformation
{
	FSSAOViewportTransformation();

	XMFLOAT4X4 ProjectionMatrix;
	XMFLOAT4X4 InversiveProjectionMatrix;
	XMFLOAT4X4 TexProjectionMatrix;

	// 设置遮蔽属性
	float OcclusionRadius;
	float OcclusionStart;
	float OcclusionEnd;
	float ObscurationThreshold;

	XMFLOAT4 SampleVolumeBuffer[14];
};

struct FSSAOBlurParam
{
	FSSAOBlurParam();

	XMFLOAT4 BlurWeights[3];
	XMFLOAT2 InversionSize;
	float BlurRadius;
};
