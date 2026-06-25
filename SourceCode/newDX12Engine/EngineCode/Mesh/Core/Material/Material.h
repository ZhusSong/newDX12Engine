#pragma once
#include "CoreObject/CoreMinimalObject.h"
#include "MaterialType.h"
#include"../../../EngineMinimal.h"
#include "Material.CodeReflection.h"

class CMaterial :public CCoreMinimalObject
{
	CODEREFLECTION()
public:
	  CMaterial();

	  // 设置材质显示状态
	  // マテリアル表示状態を設定
	  void SetMaterialDisplayStatus(EMaterialDisplayStatusType InDisplayStatus);

	  // 设置基础颜色
	  // ベースカラーを設定
	  void SetBaseColor(const fvector_4d& InBaseColor);
	  // 贴图方式
	  // テクスチャ方式
	  void SetBaseColor(const std::string& InAssetFilename);

	  // 设置基材质种类
	  // ベースマテリアルの種類を設定
	  void SetMaterialType(const EMaterialType& InMaterialType);

	  // 设置光滑度
	  // ラフネス（粗さ）を設定
	  void SetRoughness(const float InNewRoughness);

	  // 设置反射度
	  // スペキュラー（反射度）を設定
	  void SetSpecular(const std::string& InAssetFilename);
	  void SetSpecular(const fvector_3d& InVector);

	  // 设置Fresnel参数
	  // Fresnelパラメータを設定
	  void SetFresnelF0(const fvector_3d& InF0Vector);

	  // 设置透明度
	  // 透明度を設定
	  void SetTransparency(float InTransparency);

	  // 设置是否启用玻璃透明化处理
	  // ガラス用の透明処理を設定
	  void SetUseGlass(bool bInUseGlass);

	  // 设置法线贴图
	  // 法線マップを設定
	  void SetNormal(const std::string& InAssetFilename);

	  // 设置脏标志
	  // ダーティフラグを設定
	  void SetDirty(bool bNewDirty);

	  // 设置材质索引
	  // マテリアルインデックスを設定
	  void SetMaterialIndex(int InNewIndex);

	  // 设置动态反射
	  // 動的反射を設定
	  void SetDynamicReflection(bool InDynamicReflection);

	  // 设置平面反射
	  // 平面反射を設定
	  void SetPlanarReflection(bool InPlanarReflection);
	  // 设置折射率
	  // 屈折率を設定
	  void SetRefractiveValue(float InRefractiveValue);
	  // 设置金属度
	  // メタリック値を設定
	  void SetMetallicity(float InMetallicity);

	  // 自定义参数
	  // カスタムパラメータ
public:
	void SetFloatParam(int ParamIndex, float InValue);
	float GetFloatParam(int ParamIndex) const;

public:
	// 动态反射
	// 動的反射
	  FORCEINLINE float IsDynamicReflection() const {
		  return bDynamicReflection &&
			  !bPlanarReflection &&
			  (MaterialType == EMaterialType::Back ||
				  MaterialType == EMaterialType::Phong ||
				  MaterialType == EMaterialType::BinnPhong ||
				  MaterialType == EMaterialType::PBR ||
				  MaterialType == EMaterialType::Transparency);
	  }

	  FORCEINLINE float IsPlanarReflection() const {
		  return bPlanarReflection &&
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
	  FORCEINLINE bool IsUseGlass()const { return bUseGlass; }
	  FORCEINLINE bool IsUsePlanarReflection() const { return bPlanarReflection; }

	  FORCEINLINE EMaterialType GetMaterialType()const { return MaterialType; }
	  FORCEINLINE float GetRefractiveValue()const { return Refractive; }
	  FORCEINLINE fvector_3d GetMetallicity()const { return Metallicity; }
	  // 得到渲染模板
	  // レンダリングテンプレートを取得
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

	float Transparency;			// 透明度     // 透明度
	bool bUseGlass;			// 玻璃透明化处理 // ガラス透明処理

	bool bDynamicReflection;	// 动态反射   // 動的反射
	bool bPlanarReflection;		// 平面反射   // 平面反射

	fvector_3d Metallicity;		// 金属度	  // メタリック値

	float Refractive;			// 折射率     // 屈折率

// 自定义参数
// カスタムパラメータ
private:
	float Param0;
	float Param1;
	float Param2;
};
