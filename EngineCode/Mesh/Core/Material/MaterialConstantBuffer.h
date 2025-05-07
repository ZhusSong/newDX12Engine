// 材质常量缓冲区
#pragma once
#include "../../../EngineMinimal.h"

struct FMaterialConstantBuffer
{
	FMaterialConstantBuffer();
	XMFLOAT4 BaseColor;

	XMFLOAT4X4 TransformInformation;
};
