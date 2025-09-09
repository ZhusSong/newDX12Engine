#pragma once
#include "../../EngineMinimal.h"
#include "../../Core/CoreObject/CoreMinimalObject.h"

class CTransformComponent;
//G -> Game
class GActorObject :public CCoreMinimalObject
{
	CVARIABLE()
		CTransformComponent* RootComponent;

public:
	GActorObject();

public:
	FORCEINLINE CTransformComponent* GetRootComponent() const { return RootComponent; }

	// 获取碰撞盒
	void GetBoundingBox(BoundingBox& OutBoundingBox);
	BoundingBox GetBoundingBox();
public:
	virtual void SetPosition(const XMFLOAT3& InNewPosition);
	virtual void SetRotation(const fvector_3d& InRotation);

	virtual void SetRotation(const frotator& InNewRotation);
	// 设置四元数旋转
	virtual void SetRotationQuat(const fquat& InNewQuatRotation);
	virtual void SetScale(const fvector_3d& InNewScale);


	virtual void SetPickup(bool bNewPickup);
public:
	XMFLOAT3& GetPosition();

	frotator GetRotation()const;


	fvector_3d GetScale()const;

	// 四元数
	fquat GetRotationQuat() const;

	XMFLOAT3& GetForwardVector();
	XMFLOAT3& GetRightVector();
	XMFLOAT3& GetUPVector();

};