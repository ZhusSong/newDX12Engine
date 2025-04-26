#pragma once
#include "../../EngineMinimal.h"
#include "../../Core/CoreObject/CoreMinimalObject.h"

class CTransformComponent;
//G -> Game
class GActorObject :public CCoreMinimalObject
{
	CVARIABLE()
		CTransformComponent* TransformComponent;

public:
	GActorObject();

public:
	FORCEINLINE CTransformComponent* GetTransformationComponent() const { return TransformComponent; }
public:
	void SetPosition(const XMFLOAT3& InNewPosition);
	void SetRotation(const fvector_3d& InRotation);
	void SetScale(const fvector_3d& InNewScale);

public:
	XMFLOAT3& GetPosition();
	fvector_3d GetRotation()const;
	fvector_3d GetScale()const;

	XMFLOAT3& GetForwardVector();
	XMFLOAT3& GetRightVector();
	XMFLOAT3& GetUPVector();


};