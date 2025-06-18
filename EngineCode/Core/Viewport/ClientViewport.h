
#pragma once
#include "../../Core/Engine.h"
#include "Viewport.h"
#include "../../Actor/Core/ActorObject.h"

class GClientViewport :public GActorObject
	, public FViewport
{
	typedef FViewport SuperV;
	typedef GActorObject SuperA;

public:
	GClientViewport();

	// 设置视锥
	void SetFrustum(float InYFOV, float InAspect, float InZNear, float InZFar);

	// 视图控制
	void FaceTarget(const fvector_3d& InPosition, const fvector_3d& InTargetPosition, const fvector_3d& InUP = fvector_3d(0.f, 1.f, 0.f));
	// 构建视图矩阵
	virtual void BuildViewMatrix(float DeltaTime);
	virtual void Tick(float DeltaTime);

public:
	FORCEINLINE float GetFOV() const { return YFOV; }
	FORCEINLINE float GetAspect() const { return Aspect; }
	FORCEINLINE float GetNear() const { return ZNear; }
	FORCEINLINE float GetFar() const { return ZFar; }
	FORCEINLINE bool GetDirty() const { return bDirty; }

	void SetDirty(bool bNewDirty) { bDirty = bNewDirty; }
private:
	float YFOV;
	float Aspect;
	float ZNear;
	float ZFar;

	bool bDirty;
};