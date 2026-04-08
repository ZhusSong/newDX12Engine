#pragma once
#include "../../TransformComponent.h"
#include "MeshComponentType.h"
#include "MeshComponent.CodeReflection.h"
// 模型网格组件
// モデルメッシュコンポーネント
class CMaterial;
struct FMeshRenderingData;

class CMeshComponent :public CTransformComponent
{
	CODEREFLECTION()

	CVARIABLE()
		vector<CMaterial*> Materials;
public:
	CMeshComponent();

	virtual void Init();

	virtual void BuildMesh(const FMeshRenderingData* InRenderingData);
	void SetMeshRenderLayerType(EMeshRenderLayerType InRenderLayerType);

public:
	// 获取碰撞盒
	// コリジョンボックスを取得する
	void GetBoundingBox(BoundingBox& OutBoundingBox);
	BoundingBox GetBoundingBox();

public:
	UINT GetMaterialNum()const;
	void SetMaterialCount(UINT InMaterialCount);
	CMaterial* GetMaterialBySlot(UINT InMaterialSlot) const;
	virtual void ApplyCachedMeshMetadata(const size_t InMeshHash);

	EMeshRenderLayerType GetRenderLayerType()const { return MeshRenderLayerType; }

	vector<CMaterial*>* GetMaterials() { return &Materials; }

	EMeshRenderLayerType MeshRenderLayerType;


public:
	// 动态反射
	// 動的反射
	bool IsDynamicReflection() const;

	// 设置是否开启阴影
	// 影を有効にするかどうかを設定する
	void SetCastShadow(bool bNewCastShadow) { bCastShadow = bNewCastShadow; }
	bool IsCastShadow() const { return bCastShadow; }

	// 设置是否可被拾取
	// ピックアップ可能かどうかを設定する
	void SetPickup(bool bNewPickup) { bPickup = bNewPickup; }
	bool IsPickup() const { return bPickup; }

	// 设置是否可见
	// 可視状態を設定する
	void SetVisible(bool bNewVisible) { bVisible = bNewVisible; }
	bool IsVisible() const { return bVisible; }

protected:
	// 是否产生阴影
	// 影を生成するかどうか
	bool bCastShadow;
	// 是否可被拾取
	// ピックアップ可能かどうか
	bool bPickup;
	// 是否可见
	// 可視かどうか
	bool bVisible;
};
