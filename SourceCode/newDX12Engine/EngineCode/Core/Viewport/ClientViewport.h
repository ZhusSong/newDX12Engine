
#pragma once
#include "../../Core/Engine.h"
#include "Viewport.h"
#include "../../Actor/Core/ActorObject.h"
#include "../../Manager/ViewportDataManager.h"

class GClientViewport :public GActorObject
	, public FViewport
{
	typedef FViewport SuperV;
	typedef GActorObject SuperA;

public:
	GClientViewport();

	// 设置视锥
	// 視錐台を設定
	void SetFrustum(float InYFOV, float InZNear, float InZFar);
	void SetFrustum(float InYFOV, float InAspect, float InZNear, float InZFar);

	// 视图控制
	// ビュー制御
	void FaceTarget(const fvector_3d& InPosition, const fvector_3d& InTargetPosition, const fvector_3d& InUP = fvector_3d(0.f, 1.f, 0.f));

	// 构建视图矩阵
	// ビュー行列を構築
	virtual void BuildViewMatrix(float DeltaTime);
	virtual void Tick(float DeltaTime);

	virtual void OnResetSize(int InWidth, int InHeight);

	// 构建正交矩阵
	// 正射影行列を構築
	void BuildOrthographicOffCenterLHMatrix(float InRadius, const fvector_3d& InTargetPosition);
public:
	FORCEINLINE float GetFOV() const { return ViewportData.YFOV; }
	FORCEINLINE float GetAspect() const { return  ViewportData.Aspect; }
	FORCEINLINE float GetNear() const { return  ViewportData.ZNear; }
	FORCEINLINE float GetFar() const { return  ViewportData.ZFar; }
	FORCEINLINE int GetWidth() const { return ViewportData.Width; }
	FORCEINLINE int GetHeight() const { return ViewportData.Height; }
	FORCEINLINE bool GetDirty() const { return bDirty; }

	void SetDirty(bool bNewDirty) { bDirty = bNewDirty; }
private:

	FViewportDataManager ViewportData;

	bool bDirty;
};