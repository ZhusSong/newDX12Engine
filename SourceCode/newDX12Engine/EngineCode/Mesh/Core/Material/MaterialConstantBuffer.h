// 材质常量缓冲区
#pragma once
#include "../../../EngineMinimal.h"

struct FMaterialConstantBuffer
{
	FMaterialConstantBuffer();

	int MaterialType;// 4
	float Roughness;			//粗糙度
	int BaseColorIndex;			//颜色索引	
	int NormalIndex;			//法线索引

	int SpecularIndex;
	float Param0;		// 自定义
	float Param1;		// 自定义
	float Param2;		// 自定义

	XMFLOAT4 BaseColor;
	XMFLOAT3 SpecularColor;
	int Refraction;
	XMFLOAT3 FresnelF0;

	float Transparency;					// 透明度    
	XMFLOAT4X4 TransformInformation;
	XMFLOAT3 Metallicity;
	float xxx5;
};
