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
	void SetMeshRenderLayerType(EMeshRenderLayerType InRenderLayerType) { MeshRenderLayerType = MeshRenderLayerType; }

public:
	UINT GetMaterialNum()const;

	EMeshRenderLayerType GetRenderLayerType()const { return MeshRenderLayerType; }

	vector<CMaterial*>* GetMaterials() { return &Materials; }

	EMeshRenderLayerType MeshRenderLayerType;
};
