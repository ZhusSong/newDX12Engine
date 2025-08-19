#pragma once
#include "../../../Core/CoreObject/CoreMinimalObject.h"
#include "MaterialType.h"

class CMaterial :public CCoreMinimalObject
{
public:	
	  CMaterial();

	  // 设置材质显示状态
	  void SetMaterialDisplayStatus(EMaterialDisplayStatusType InDisplayStatus);
	 
	  // 设置基础颜色
	  void SetBaseColor(const fvector_4d& InBaseColor);
	  // 贴图方式
	  void SetBaseColor(const std::string& InAssetFilename);

	  // 设置基材质种类
	  void SetMaterialType(const EMaterialType& InMaterialType);

	  // 设置光滑度
	  void SetRoughness(const float InNewRoughness);

	  // 设置反射度
	  void SetSpecular(const std::string& InAssetFilename);
	  void SetSpecular(const fvector_3d& InVector);

	  // 设置Fresnel参数
	  void SetFresnelF0(const fvector_3d& InF0Vector);

	  // 设置透明度
	  void SetTransparency(float InTransparency);

	  // 设置法线贴图
	  void SetNormal(const std::string& InAssetFilename);

	  // 设置脏标志
	  void SetDirty(bool bNewDirty);

	  // 设置材质索引
	  void SetMaterialIndex(int InNewIndex);

	  // 设置动态反射
	  void SetDynamicReflection(bool InDynamicReflection);
	  // 设置折射率
	  void SetRefractiveValue(float InRefractiveValue); 
	  // 设置金属度
	  void SetMetallicity(float InMetallicity);

	  //自定义参数
public:
	void SetFloatParam(int ParamIndex, float InValue);
	float GetFloatParam(int ParamIndex) const;

public:
	  // 动态反射
	  FORCEINLINE float IsDynamicReflection() const {
		  return bDynamicReflection &&
			  (MaterialType == EMaterialType::Back ||
				  MaterialType == EMaterialType::Phong ||
				  MaterialType == EMaterialType::BinnPhong ||
				  MaterialType == EMaterialType::PBR ||
				  MaterialType == EMaterialType::Transparency);
	  }
	  

	  FORCEINLINE float GetRoughness()const { return Roughness; }
	  FORCEINLINE fvector_4d GetBaseColor()const { return BaseColor; }

	  FORCEINLINE fvector_3d GetSpecularColor()const { return SpecularColor; }

	  FORCEINLINE float GetTransparency()const { return Transparency; }
	  FORCEINLINE fvector_3d GetFresnelF0()const { return FresnelF0; }
	  
	  FORCEINLINE EMaterialType GetMaterialType()const { return MaterialType; }
	  FORCEINLINE float GetRefractiveValue()const { return Refractive; }
	  FORCEINLINE fvector_3d GetMetallicity()const { return Metallicity; }
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
	  FORCEINLINE const std::string& GetSpecularKey() const { return SpecularIndexKey; }

	  FORCEINLINE int GetMaterialIndex() const { return MaterialIndex; }
private:
	bool bDirty;
	int MaterialIndex;

	fvector_4d BaseColor;
	fvector_3d SpecularColor;
	fvector_3d FresnelF0;

	float	   Roughness;
	EMaterialType MaterialType;

	std::string BaseColorIndexKey;
	std::string NormalIndexKey;
	std::string SpecularIndexKey;

	EMaterialDisplayStatusType MaterialDisplayStatus;
	XMFLOAT4X4 MaterialTransform;

	float Transparency;			//透明度

	bool bDynamicReflection;  	//动态反射

	fvector_3d Metallicity;		//金属度

	
	float Refractive;			//折射率

	//自定义参数
private:
	float Param0;//自定义
	float Param1;//自定义
	float Param2;//自定义
};