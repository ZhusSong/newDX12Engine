#pragma once

#include "../../Windows/WindowsPlatform.h"
#include "../../CodeReflection/CodeReflectionMacroTag.h"
#include "GuidInterface.h"
#include "../Construction/ObjectConstruction.h"

class CCoreMinimalObject;

//提供核心对象接口
class CCoreMinimalObject :public IGuidInterface
{
public:
	CCoreMinimalObject();
	virtual ~CCoreMinimalObject();

	virtual void BeginInit() {};
	virtual void Tick(float DeltaTime) {};


	bool IsTick()const { return bTick; }


	FORCEINLINE CCoreMinimalObject* GetOuter() const { return Outer; }
	void SetOuter(CCoreMinimalObject* InNewOuter) { Outer = InNewOuter; }
protected:
	bool bTick;


	// 外层对象
	CCoreMinimalObject* Outer;

};

extern vector<CCoreMinimalObject*> GObjects;