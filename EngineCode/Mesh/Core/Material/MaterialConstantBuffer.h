// 材质常量缓冲区
#pragma once
#include "../../../EngineMinimal.h"

struct FMaterialConstantBuffer
{
	FMaterialConstantBuffer();

	int MaterialType;// 4
	
	// 粗糙度
	float	 Roughness;
	// 颜色索引
	int BaseColorIndex;
	// 法线索引
	int NormalIndex;

	XMFLOAT4 BaseColor;

	XMFLOAT4X4 TransformInformation;
};
