#pragma once

#include "../../Windows/WindowsPlatform.h"
#include "../../CodeReflection/CodeReflectionMacroTag.h"
#include "GuidInterface.h"
#include "../Construction/ObjectConstruction.h"

//提供核心对象接口
class CCoreMinimalObject :public IGuidInterface
{
public:
	CCoreMinimalObject();
	virtual ~CCoreMinimalObject();

	virtual void BeginInit() {};
	virtual void Tick(float DeltaTime) {};


	bool IsTick()const { return bTick; }
protected:
	bool bTick;
};

extern vector<CCoreMinimalObject*> GObjects;