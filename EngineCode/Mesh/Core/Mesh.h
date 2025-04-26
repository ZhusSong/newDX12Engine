#pragma once
#include "../../Rendering/Core/Rendering.h"
#include "MeshType.h"
#include "../../Actor/Core/ActorObject.h"

class CTransformComponent;
// 游戏对象网格接口
class GMesh :public GActorObject, public IRenderingInterface
{
	CVARIABLE()
		CTransformComponent* TransformComponent;
public:
	GMesh();

	virtual void Init();

	virtual void BuildMesh(const FMeshRenderingData* InRenderingData);

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);
};