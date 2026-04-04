#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/CodeReflectionMacroTag.h"
#include "../EngineType.h"
#include "World.CodeReflection.h"

struct FInputKey;
class CTransformComponent;
class CInputComponent;
class GCamera;
class GActorObject;

class CWorld :public CCoreMinimalObject
{
	CODEREFLECTION()
public:
	CWorld();

	GCamera* GetCamera() const { return Camera; }

	// 创建Actor对象
	// Actorオブジェクトを作成
	template<class T>
	T* CreateActorObject()
	{
		T* InArray = new T();
		InArray->SetOuter(this);

		InArray->InitMinimalObject();


		char ObjectName[128] = { 0 };
		sprintf(ObjectName, "%s_%d",
			InArray->GetName().c_str(),
			ActorObjects.size());

		InArray->Rename(ObjectName);

		ActorObjects.push_back(InArray);

		return InArray;
	}
public:
	// 射线检测
	// レイキャスト判定
	bool LineTraceBySingle(FCollisionResult& OutResult, const fvector_3d& InStart, const fvector_3d& InEnd);

	const vector<GActorObject*>& GetActors() const { return ActorObjects; }
protected:
	CVARIABLE()
		GCamera* Camera;

	// Actor对象池
	// Actorオブジェクトプール
	CVARIABLE()
		vector<GActorObject*> ActorObjects;
};