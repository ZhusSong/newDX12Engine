#pragma once
#include "../../../Core/CoreObject/CoreMinimalObject.h"
#include "MaterialType.h"

class CMaterial :public CCoreMinimalObject
{
public:	CMaterial();

	  // 设置材质显示状态
	  void SetMaterialDisplayStatus(EMaterialDisplayStatusType InDisplayStatus);
	 
	  // 设置基础颜色
	  void SetBaseColor(const fvector_4d& InBaseColor);

	  // 设置基材质种类
	  void SetMaterialType(const EMaterialType& InMaterialType);

	  void SetBaseColor(const std::string& InAssetFilename);
	  void SetNormal(const std::string& InAssetFilename);
	  void SetDirty(bool bNewDirty);
	  void SetMaterialIndex(int InNewIndex);

	  // 设置光滑度
	  void SetRoughness(const float InNewRoughness);

	  FORCEINLINE float GetRoughness()const { return Roughness; }
	  FORCEINLINE fvector_4d GetBaseColor()const { return BaseColor; }

	  FORCEINLINE EMaterialType GetMaterialType()const { return MaterialType; }


	  // 得到渲染模板
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

	  FORCEINLINE XMFLOAT4X4& GetMaterialTransform() { return MaterialTransform; }
	  FORCEINLINE bool IsDirty() const { return bDirty; }
	  FORCEINLINE const std::string& GetBaseColorIndexKey() const { return BaseColorIndexKey; }
	  FORCEINLINE const std::string& GetNormalIndexKey() const { return NormalIndexKey; }
	  FORCEINLINE int GetMaterialIndex() const { return MaterialIndex; }
private:
	bool bDirty;
	int MaterialIndex;
	fvector_4d BaseColor;
	float	   Roughness;
	EMaterialType MaterialType;

	std::string BaseColorIndexKey;
	std::string NormalIndexKey;

	EMaterialDisplayStatusType MaterialDisplayStatus;
	XMFLOAT4X4 MaterialTransform;
};