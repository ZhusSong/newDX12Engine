#pragma once
#include "Core/OperationHandleBase.h"
#include "RotatorArrow.CodeReflection.h"

class CPlaneMeshComponent;
class CMeshComponent;
class GRotatorArrow :public GOperationHandleBase
{
	CODEREFLECTION()

	CVARIABLE()
		CPlaneMeshComponent* XPlaneComponent;

	CVARIABLE()
		CPlaneMeshComponent* YPlaneComponent;

	CVARIABLE()
		CPlaneMeshComponent* ZPlaneComponent;

public:
	GRotatorArrow();

	void CreateMesh();

	void LoadPlaneComponentMaterial(CMeshComponent* InComponent, const std::string& InFilename);

protected:
	// 旋转指示面片(Angle Guide)相关
	// 回転ガイド面（Angle Guide）関連
	void SetAGValue(CMeshComponent* InComponent, float InValue);
	void SetAGValue(float InValue);
	void ResetAGValue();

	virtual fvector_3d GetSelectedObjectDirection(
		const fvector_3d& WorldOriginPoint,
		const fvector_3d& WorldDirection,
		const fvector_3d& ActorWorldPosition);

protected:
	virtual void Tick(float DeltaTime);
	virtual void BeginInit();
public:
	virtual void ResetVisible(CCustomMeshComponent* InAxisComponent, bool bVisible);
	virtual void ResetVisible();
	virtual void SetVisible(bool bNewVisible);
protected:
	virtual void SetScale(const fvector_3d& InNewScale);
	virtual void SetPosition(const XMFLOAT3& InNewPosition);
protected:
	virtual void OnMouseMove(int X, int Y);
	virtual void OnLeftMouseButtonDown(int X, int Y);
	virtual void OnLeftMouseButtonUp(int X, int Y);
	virtual void OnCaptureKeyboardInfor(const FInputKey& InKey);

protected:
	float GetSymbol(float InValueOffset, bool bFlip = false);
	float GetSymbolByCubeIndex(float InValueOffset);
	float GetSymbolMaterialByCubeIndex(float InValueOffset);

	float GetAngleRatio(float InAngle);
	void PrintAix(ESelectAxisType InAxisType);

protected:
	// 上一次拖拽的值
	float LastT2Value;
	// 上一次的旋转变化量
	float RotatorRatio;
	// 立方体索引
	int Sample8CubeIndex;
};