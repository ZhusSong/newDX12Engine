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


	  FORCEINLINE D3D_PRIMITIVE_TOPOLOGY GetMaterialDisplayStatus()const {
		  switch (MaterialDisplayStatus){

		  case EMaterialDisplayStatusType::DefaultDisplay:
				  return  D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		  case EMaterialDisplayStatusType::PointDisplay:
			  return  D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		  case EMaterialDisplayStatusType::WireframeDisplay:
			  return  D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		  case EMaterialDisplayStatusType::TriangleDisplay:
			  return  D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		  default:
			  return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		  }
	  }
private:
	fvector_4d BaseColor;
	EMaterialType MaterialType;
	float	   Roughness;

	EMaterialDisplayStatusType MaterialDisplayStatus;
};