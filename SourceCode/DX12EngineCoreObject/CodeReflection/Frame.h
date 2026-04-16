#pragma once

#include "../DX12EngineCoreObjectMacro.h"
#include "../CoreObjectMinimal.h"
#include "../CoreObject/CoreMinimalObject.h"

class CFunctionObject;
class CPropertyObject;

class FOutParm :public std::enable_shared_from_this<FOutParm>
{
public:
	FOutParm();

	CPropertyObject* PropAddr;
	std::shared_ptr<FOutParm> Next;
};

struct FFrame
{
	FFrame();
	FFrame(CFunctionObject* InNewFunction);

	template<typename Type>
	Type Read();

	float ReadFloat();
	int ReadInt();

	void AddParm(CPropertyObject* Addr);

	void Step(CCoreMinimalObject* Context, void const* RefData);

	RI_ENGINECOREOBJECT_API unsigned char* GetParmAddr();
public:
	unsigned char* Code;
	CFunctionObject* Function;
	std::shared_ptr<FOutParm> OutParm;//リンクリスト
};

typedef void(*FNativeFuntionPtr)(CCoreMinimalObject* Context, FFrame& TheStack, void const* RefData);

template<typename Type>
inline Type FFrame::Read()
{
	Type Result = 0;
	int TypeLen = sizeof(Type);

	memcpy(&Result, Code, TypeLen);

	Code += TypeLen;

	return Result;
}
