#include "CoreMinimalObject.h"
#include "FunctionObject.h"
#include "PropertyObject.h"
#include "../CodeReflection/Frame.h"

// 全局对象池
// グローバルオブジェクトプール
vector<CCoreMinimalObject*> GObjects;


CCoreMinimalObject::CCoreMinimalObject()
{
	bTick = true;
	GObjects.push_back(this);
	Outer = NULL;
}

CCoreMinimalObject::~CCoreMinimalObject()
{
	for (vector<CCoreMinimalObject*>::const_iterator Iter = GObjects.begin();
		Iter != GObjects.end();
		++Iter)
	{
		if (*Iter == this)
		{
			GObjects.erase(Iter);
			break;
		}
	}
}

void CCoreMinimalObject::CallFunction(FFrame& Stack, void const* Data, CFunctionObject* Function)
{
	// 收集函数的参数
	// 関数のパラメータを収集
	CPropertyObject* PropertyPtr = Function->Property;
	while (PropertyPtr)
	{
		Stack.AddParm(PropertyPtr);
		PropertyPtr = dynamic_cast<CPropertyObject*>(PropertyPtr->Nest);
	}



	// 执行字节码对应的命令
	// バイトコードに対応するコマンドを実行
	Stack.Step(NULL, Data);
}

void CCoreMinimalObject::ExecutionScript(CFunctionObject* Function, void const* Data)
{
	assert(Function);

	if (Function->Script.size() == 0)
	{
		return;
	}

	FFrame Stack(Function);

	CCoreMinimalObject::CallFunction(Stack, Data, Function);
}

CFunctionObject* CCoreMinimalObject::FindScriptFuntion(const std::string& FunName)
{
	CFunctionObject* NewFuntion = nullptr;
	auto It = FunctionList.find(FunName);
	if (It != FunctionList.end())
	{
		NewFuntion = It->second;
	}

	return NewFuntion;
}

