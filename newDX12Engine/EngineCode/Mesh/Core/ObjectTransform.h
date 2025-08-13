// 2025.4.李
#pragma once
#include "MeshType.h"

struct FObjectTransform
{
	FObjectTransform();

	// 当前对象的世界坐标
	XMFLOAT4X4 World;

	XMFLOAT4X4 TextureTransformation;
	UINT MaterialIndex;
	// 占位
	UINT VV1;
	UINT VV2;
	UINT VV3;

};
