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
	int XX1;
	int XX2;
	int XX3;

	XMFLOAT4 BaseColor;
	XMFLOAT3 SpecularColor;
	int XX6;
	XMFLOAT3 FresnelF0;

	float Transparency;					// 透明度    
	XMFLOAT4X4 TransformInformation;
};
