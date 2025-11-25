#pragma once

#include "../DX12EngineCoreObjectMacro.h"
#include "../CoreObjectMinimal.h"
#include "../CoreObject/CoreMinimalObject.h"
#include "Frame.h"

class CFunctionObject;

struct RI_ENGINECOREOBJECT_API  FFuntionID
{
	std::string ClassName;
	std::string FuncName;

	FNativeFuntionPtr NativeFuncPtr;

	FFuntionID(const std::string& InNewClassName, const std::string& InNewFuncName, FNativeFuntionPtr NewNativeFuncPtr)
		:ClassName(InNewClassName)
		, FuncName(InNewFuncName)
		, NativeFuncPtr(NewNativeFuncPtr)
	{}
};

struct RI_ENGINECOREOBJECT_API  FFuntionManager
{
	static FNativeFuntionPtr& GetNativeFuncPtr(CFunctionObject* Function);
	static int SetNativeFuncPtr(const FFuntionID& Function);
private:
	static vector<FFuntionID> FunctionList;
};