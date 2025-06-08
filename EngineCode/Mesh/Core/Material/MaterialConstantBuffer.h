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

	int SpecularIndex;
	int XX1;
	int XX2;
	int XX3;

	XMFLOAT4 BaseColor;
	XMFLOAT3 SpecularColor;
	int XX6;

	XMFLOAT4X4 TransformInformation;
};
