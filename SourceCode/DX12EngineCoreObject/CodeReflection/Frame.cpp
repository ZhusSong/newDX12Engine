#include "../CoreObject/CoreMinimalObject.h"
#include "Frame.h"
#include "ScriptCommand.h"
#include "../CodeReflection/ScriptMacro.h"

FNativeFuntionPtr GScriptNatives[Script_Max];

FFrame::FFrame()
{
}

FFrame::FFrame(CFunctionObject* InNewFunction)
{
}

float FFrame::ReadFloat()
{
	return Read<float>();
}

int FFrame::ReadInt()
{
	return Read<int>();
}

void FFrame::AddParm(CPropertyObject* Addr)
{
	std::shared_ptr<FOutParm> ListParm = OutParm;

	if (!ListParm.get())
	{
		OutParm = std::make_shared<FOutParm>();
		OutParm->PropAddr = Addr;
	}
	else
	{
		while (ListParm->Next.get())
		{
			ListParm = ListParm->Next;
		}

		ListParm->Next = std::make_shared<FOutParm>();
		ListParm->Next->PropAddr = Addr;
	}
}

void FFrame::Step(CCoreMinimalObject* Context, void const* RefData)
{
	int B = *Code++;
	(GScriptNatives[B])(Context, *this, RefData);
}
unsigned char* FFrame::GetParmAddr()
{
	CPropertyObject* Proper = OutParm->PropAddr;
	OutParm = OutParm->Next;



	return nullptr;
}

FOutParm::FOutParm()
	:PropAddr(nullptr)
{
}

FUNCTION_IMPLEMENTATION(CCoreMinimalObject::Script_Undefined)
{

}

FUNCTION_IMPLEMENTATION(CCoreMinimalObject::Script_Int)
{
	*(int*)RefData = Stack.ReadInt();
}

FUNCTION_IMPLEMENTATION(CCoreMinimalObject::Script_Float)
{
	*(float*)RefData = Stack.ReadFloat();
}

FUNCTION_IMPLEMENTATION(CCoreMinimalObject::Script_Funtion)
{

}
void CCoreMinimalObject::InitMinimalObject()
{
	InitReflectionContent();
}