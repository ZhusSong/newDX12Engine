#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "../CodeReflection/CodeReflectionMacroTag.h"

struct FInputKey;
class CTransformComponent;
class CInputComponent;
class GCamera;

class CWorld :public CCoreMinimalObject
{
public:
	CWorld();

	GCamera* GetCamera() { return Camera; }

protected:
	CVARIABLE()
		GCamera* Camera;
};