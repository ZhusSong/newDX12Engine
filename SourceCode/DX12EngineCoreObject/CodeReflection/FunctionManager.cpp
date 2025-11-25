#include "FunctionManager.h"
#include "../CoreObject/FunctionObject.h"

vector<FFuntionID> FFuntionManager::FunctionList;

namespace NativeFuntionMess
{
	void Error(CCoreMinimalObject* Context, FFrame& TheStack, void const* RefData)
	{
		assert(0);
	}

	void Empty(CCoreMinimalObject* Context, FFrame& TheStack, void const* RefData)
	{
	}
}

FNativeFuntionPtr ErrorFunction = NativeFuntionMess::Error;
FNativeFuntionPtr EmptyFunction = NativeFuntionMess::Empty;

FNativeFuntionPtr& FFuntionManager::GetNativeFuncPtr(CFunctionObject* Function)
{
	for (auto& Func : FunctionList)
	{
		if (Function->GetName() == Func.FuncName && //函数名
			Function->GetOuter()->GetName() == Func.ClassName) //类名
		{
			return Func.NativeFuncPtr;
		}
	}

	return EmptyFunction;
}

int FFuntionManager::SetNativeFuncPtr(const FFuntionID& Function)
{
	FunctionList.push_back(Function);

	return 0;
}
