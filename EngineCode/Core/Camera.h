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
	FORCEINLINE 	CInputComponent* GetInputComponent() { return InputComponent; }

	FORCEINLINE 	CTransformComponent* GetTransformationComponent() { return TransformationComponent; }
};