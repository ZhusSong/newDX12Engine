#pragma once

#include "../CodeReflection/CodeReflectionMacroTag.h"
#include "GuidInterface.h"
#include "../Construction/ObjectConstruction.h"

#include "../CodeReflection/ScriptCommand.h"
#include "../CodeReflection/ScriptMacro.h"


class CCoreMinimalObject;

class CFunctionObject;
struct FFrame;

// 提供核心对象接口
// コアオブジェクトインターフェースを提供
class RI_ENGINECOREOBJECT_API CCoreMinimalObject :public IGuidInterface
{
public:
	CCoreMinimalObject();
	virtual ~CCoreMinimalObject();

	virtual void BeginInit() {};
	virtual void Tick(float DeltaTime) {};


	bool IsTick()const { return bTick; }


	FORCEINLINE CCoreMinimalObject* GetOuter() const { return Outer; }

	FORCEINLINE std::string GetName() const { return Name; }
	void SetOuter(CCoreMinimalObject* InNewOuter) { Outer = InNewOuter; }
	void Rename(const std::string& InName) { Name = InName; }

	// 字节码表函数对应的内容
	// バイトコードテーブルの関数に対応する内容
public:
	FUNCTION_DEFINITION(Script_Undefined);
	FUNCTION_DEFINITION(Script_Int);
	FUNCTION_DEFINITION(Script_Float);
	FUNCTION_DEFINITION(Script_Funtion);

	// 函数操作
	// 関数操作
public:
	static void CallFunction(FFrame& Stack, void const* Data, CFunctionObject* Function);
	static void ExecutionScript(CFunctionObject* Function, void const* Data);

	// 该函数可以获取编译后带字节码的函数
	// この関数はコンパイル後のバイトコード付き関数を取得できます
	CFunctionObject* FindScriptFuntion(const std::string& FunName);
protected:
	bool bTick;


	// 外层对象
	// 外部オブジェクト
	CCoreMinimalObject* Outer;
	std::string Name;

	// 函数列表
	// 関数リスト
	std::map<std::string, CFunctionObject*> FunctionList;
};


extern  RI_ENGINECOREOBJECT_API  vector<CCoreMinimalObject*> GObjects;