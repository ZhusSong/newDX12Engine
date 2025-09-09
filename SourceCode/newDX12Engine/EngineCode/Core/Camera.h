#pragma once
#include "Viewport/Viewport.h"
#include "Viewport/ClientViewport.h"
#include "../CodeReflection/CodeReflectionMacroTag.h"
#include "../Interface/DirectXDeviceInterface.h"
#include "../Component/TimelineComponent.h"

enum ECmeraType;
struct FInputKey;
class CTransformComponent;
class CInputComponent;

// 相机类
class GCamera:public GClientViewport
	, public IDirectXDeviceInterface
{
	typedef GClientViewport Super;
	//CVARIABLE()
	//CTransformComponent* TransformationComponent;

	CVARIABLE()
	CInputComponent* InputComponent;
public:
	GCamera();
	virtual void BeginInit();
	virtual void Tick(float DeltaTime);

	virtual void ExecuteKeyboard(const FInputKey& InputKey);
	virtual void BuildViewMatrix(float DeltaTime);
public:
	// 鼠标处理相关
	virtual void OnLeftMouseButtoUP(int X, int Y);
	virtual void OnLeftMouseButtonDown(int X, int Y);
	virtual void OnRightMouseButtonDown(int X, int Y);
	virtual void OnRightMouseButtonUp(int X, int Y);
	virtual void OnMouseMove(int X, int Y);
	virtual void OnMouseWheel(int X, int Y, float InDelta);

	virtual void MoveForward(float InValue);
	virtual void MoveRight(float InValue);

public:
	// 点击屏幕时
	virtual void OnClickedScreen(int X, int Y);


protected:
	void RotateAroundXAxis(float InRotateDegrees);
	void RotateAroundYAxis(float InRotateDegrees);

protected:
	// 返回选中目标
	void LookAtAndMoveToSelectedObject(float InTime, float InDeltaTime);

public:
	FORCEINLINE 	CInputComponent* GetInputComponent() { return InputComponent; }

	//FORCEINLINE 	CTransformComponent* GetTransformationComponent() { return TransformationComponent; }
protected:
	POINT LastMousePosition;
	bool bLeftMouseDown;
	bool bRightMouseDown;


	float MouseSensitivity;
	ECmeraType CmeraType;

	float Radius;
	float A;//Theta
	float B;//

	class CMeshComponent* SphereMesh;

	fvector_3d FocusPoint;

	FTimeline Timeline;
	bool bFPress;
};