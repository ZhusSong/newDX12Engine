#pragma once
#include "Component/Component.h"
#include "../EngineMinimal.h"


class CTransformComponent :public CComponent
{public:
	CTransformComponent();


	virtual void SetPosition(const XMFLOAT3& InNewPosition);
	
	virtual void SetRotation(const fvector_3d& InNewRotation);//add 旋转
	virtual void SetRotation(const frotator& InNewRotation);

	// 四元数旋转
	virtual void SetRotationQuat(const fquat& InNewQuatRotation);
	virtual void SetScale(const fvector_3d& InNewScale);

	virtual void SetForwardVector(const XMFLOAT3& InForwardVector);
	
	virtual void SetRightVector(const XMFLOAT3& InRightVector);
	
	virtual void SetUPVector(const XMFLOAT3& InUPVector);

public:
	XMFLOAT3& GetPosition() { return Position; }
	
	frotator GetRotation() const;
	fvector_3d GetScale() const { return fvector_3d(Scale.x, Scale.y, Scale.z); }
	
	// 四元数旋转
	fquat GetRotationQuat() const;
	
	XMFLOAT3& GetForwardVector() { return ForwardVector; }
	
	XMFLOAT3& GetRightVector() { return RightVector; }

	XMFLOAT3& GetUPVector() { return UPVector; }


public:
	void CorrectionVector();

	fvector_3d GetCorrectionPosition();

	void GetCorrectionPosition(fvector_3d& InPos3);
private:
	XMFLOAT3 Position;
	XMFLOAT3 Rotation;
	XMFLOAT3 Scale;

	XMFLOAT3 ForwardVector;
	XMFLOAT3 RightVector;
	XMFLOAT3 UPVector;
};