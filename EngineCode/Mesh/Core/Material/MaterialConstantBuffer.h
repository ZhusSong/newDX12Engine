// 材质常量缓冲区
#pragma once
#include "../../../EngineMinimal.h"

struct FMaterialConstantBuffer
{
	FMaterialConstantBuffer();

	int MaterialType;// 4
	// 字节保留项
	int Reserved1;
	int Reserved2;
	int Reserved3;

	XMFLOAT4 BaseColor;

	XMFLOAT4X4 TransformInformation;
};
