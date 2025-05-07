#pragma once
#include "../../Rendering/Core/Rendering.h"
#include "MeshType.h"
#include "../../Actor/Core/ActorObject.h"

class CTransformComponent;
class CMaterial;
// 游戏对象网格接口
class GMesh :public GActorObject, public IRenderingInterface
{
	CVARIABLE()
		CTransformComponent* TransformComponent;

	CVARIABLE()
		vector<CMaterial*> Materials;
public:
	GMesh();

	virtual void Init();

	virtual void BuildMesh(const FMeshRenderingData* InRenderingData);

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);

public:
	// 获取材质数量
	UINT GetMaterialNum()const;
};