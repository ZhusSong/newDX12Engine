#pragma once
#include "../../../Core/CoreObject/CoreMinimalObject.h"
#include "MaterialType.h"

class CMaterial :public CCoreMinimalObject
{
public:	CMaterial();

	  // 设置材质显示状态
	  void SetMaterialDisplayStatus(EMaterialDisplayStatusType InDisplayStatus) { MaterialDisplayStatus = InDisplayStatus; }

	  // 设置基础颜色
	  void SetBaseColor(const fvector_4d& InBaseColor) { BaseColor = InBaseColor; }

	  // 设置基材质种类
	  void SetMaterialType(const EMaterialType& InMaterialType) { MaterialType = InMaterialType; }

	  // 设置光滑度
	  void SetRoughness(const float InNewRoughness) { Roughness = InNewRoughness; }

	  FORCEINLINE float GetRoughness()const { return Roughness; }
	  FORCEINLINE fvector_4d GetBaseColor()const { return BaseColor; }

	  FORCEINLINE EMaterialType GetMaterialType()const { return MaterialType; }


	  FORCEINLINE EMaterialDisplayStatusType GetMaterialDisplayStatus()const { return MaterialDisplayStatus; }
private:
	fvector_4d BaseColor;
	EMaterialType MaterialType;
	float	   Roughness;

	EMaterialDisplayStatusType MaterialDisplayStatus;
};