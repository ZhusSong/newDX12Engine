#pragma once
#include "Viewport.h"
#include "CoreObject/CoreMinimalObject.h"
#include "../CodeReflection/CodeReflectionMacroTag.h"

struct FInputKey;
class CTransformComponent;
class CInputComponent;

class CCamera:public CCoreMinimalObject,public FViewport
{
	CVARIABLE()
	CTransformComponent* TransformationComponent;

	CVARIABLE()
	CInputComponent* InputComponent;
public:
	CCamera();
	virtual void BeginInit();
	virtual void Update(float DeltaTime);

	virtual void ExecuteKeyboard(const FInputKey& InputKey);
public:
	// 鼠标处理相关
	virtual void OnMouseButtonDown(int X, int Y);
	virtual void OnMouseButtonUp(int X, int Y);
	virtual void OnMouseMove(int X, int Y);
	virtual void OnMouseWheel(int X, int Y, float InDelta);

	virtual void MoveForward(float InValue);
	virtual void MoveRight(float InValue);

public:
	FORCEINLINE 	CInputComponent* GetInputComponent() { return InputComponent; }

	FORCEINLINE 	CTransformComponent* GetTransformationComponent() { return TransformationComponent; }
protected:
	POINT LastMousePosition;
	bool bLeftMouseDown;

	float MouseSensitivity;

};