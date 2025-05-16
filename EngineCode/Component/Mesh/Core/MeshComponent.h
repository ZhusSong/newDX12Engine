// 25.5.16 李
#pragma once
#include "../../TransformComponent.h"

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
public:
	UINT GetMaterialNum()const;

	vector<CMaterial*>* GetMaterials() { return &Materials; }
};
