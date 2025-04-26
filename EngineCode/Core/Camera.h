#pragma once
#include "../Actor/Core/ActorObject.h"
#include "Viewport/Viewport.h"
#include "../CodeReflection/CodeReflectionMacroTag.h"
#include "../Interface/DirectXDeviceInterfece.h"

enum ECmeraType;
struct FInputKey;
class CTransformComponent;
class CInputComponent;

// 相机类
class GCamera:public GActorObject
	,public FViewport
	, public IDirectXDeviceInterface
{
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
	virtual void OnMouseButtonDown(int X, int Y);
	virtual void OnMouseButtonUp(int X, int Y);
	virtual void OnMouseMove(int X, int Y);
	virtual void OnMouseWheel(int X, int Y, float InDelta);

	virtual void MoveForward(float InValue);
	virtual void MoveRight(float InValue);
protected:
	void RotateAroundYAxis(float InRotateDegrees);
	void RotateAroundZAxis(float InRotateDegrees);

public:
	FORCEINLINE 	CInputComponent* GetInputComponent() { return InputComponent; }

	//FORCEINLINE 	CTransformComponent* GetTransformationComponent() { return TransformationComponent; }
protected:
	POINT LastMousePosition;
	bool bLeftMouseDown;

	float MouseSensitivity;
	ECmeraType CmeraType;

	float Radius;
	float A;//Theta
	float B;//
};