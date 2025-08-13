// 25.5.16 李
#pragma once
#include "../../TransformComponent.h"
#include "MeshComponentType.h"

// 模型网格组件
class CMaterial;
struct FMeshRenderingData;

class CMeshComponent :public CTransformComponent
{
	CVARIABLE()
		vector<CMaterial*> Materials;
public:
	CMeshComponent();

	virtual void Init();

	virtual void BuildMesh(const FMeshRenderingData* InRenderingData);
	void SetMeshRenderLayerType(EMeshRenderLayerType InRenderLayerType);

public:
	UINT GetMaterialNum()const;

	EMeshRenderLayerType GetRenderLayerType()const { return MeshRenderLayerType; }

	vector<CMaterial*>* GetMaterials() { return &Materials; }

	EMeshRenderLayerType MeshRenderLayerType;


public:
	//动态反射
	bool IsDynamicReflection() const;

	// 设置是否开启阴影
	void SetCastShadow(bool bNewCastShadow) { bCastShadow = bNewCastShadow; }
	bool IsCastShadow()const { return bCastShadow; }

	// 设置是否可被拾取
	void SetPickup(bool bNewPickup) { bPickup = bNewPickup; }
	bool IsPickup()const { return bPickup; }

protected:
	// 是否产生阴影
	bool bCastShadow;
	// 是否可被拾取
	bool bPickup;
};
