#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "../CodeReflection/CodeReflectionMacroTag.h"

struct FInputKey;
class CTransformComponent;
class CInputComponent;
class CCamera;

class CWorld :public CCoreMinimalObject
{
public:
	CWorld();

	CVARIABLE()
		CCamera* Camera;
};