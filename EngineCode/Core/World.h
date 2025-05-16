// 2025.4.李
#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "../CodeReflection/CodeReflectionMacroTag.h"

struct FInputKey;
class CTransformComponent;
class CInputComponent;
class GCamera;
class GActorObject;

class CWorld :public CCoreMinimalObject
{
public:
	CWorld();

	GCamera* GetCamera() { return Camera; }

	template<class T>
	T* CreateActorObject()
	{
		T* InArray = new T();
		ActorObjects.push_back(InArray);

		return InArray;
	}


protected:
	CVARIABLE()
		GCamera* Camera;

	// Actor对象池
	CVARIABLE()
		vector<GActorObject*> ActorObjects;
};