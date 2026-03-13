// 材质常量缓冲区
// マテリアル定数バッファ
#pragma once
#include "../../../EngineMinimal.h"

struct FMaterialConstantBuffer
{
	FMaterialConstantBuffer();

	int MaterialType;// 4
	float Roughness;			// 粗糙度      	// ラフネス（粗さ）

	int BaseColorIndex;			// 颜色索引     // カラーインデックス	

	int NormalIndex;			// 法线索引     // 法線インデックス

	int SpecularIndex;			// 高光索引     // スペキュラーインデックス

	float Param0;		// 自定义       	// カスタム

	float Param1;		// 自定义          // カスタム

	float Param2;		// 自定义         	// カスタム

	XMFLOAT4 BaseColor;
	XMFLOAT3 SpecularColor;
	int Refraction;
	XMFLOAT3 FresnelF0;

	float Transparency;					// 透明度     
	XMFLOAT4X4 TransformInformation;
	XMFLOAT3 Metallicity;
	float xxx5;
};
