/*===========================================================================
    Generated code exported from c f.
    Generated Time: 2026-02-03 14:44:24
===========================================================================*/
#include "J:/newDX12Project/newDX12Engine/SourceCode/newDX12Engine/EngineCode/Actor/Core/ActorObject.h"
#include "ActorObject.CodeReflection.h"
#include "CodeReflection/FunctionManage.h"

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS

static std::string Name_Hello123 = std::string(("Hello123"));
void   GActorObject::Hello123( int  c, float  b)
{
	struct FParm_Hello123
	{
		 int  c;
		 float  b;
	};
	FParm_Hello123 Parm_Hello123;
	Parm_Hello123.c = c;
	Parm_Hello123.b = b;
	ExecutionScript(FindScriptFuntion(Name_Hello123),&Parm_Hello123);
} 
static std::string Name_Hello = std::string(("Hello"));
void   GActorObject::Hello()
{
	ExecutionScript(FindScriptFuntion(Name_Hello),NULL);
} 
 
/* 1xxxx xxxx 
 2xxxx xxxx */
int Register_GActorObject()
{
	FFuntionManage::SetNativeFuncPtr(FFuntionID(("ActorObject"),("Hello1"),GActorObject::Script_Hello1));
	FFuntionManage::SetNativeFuncPtr(FFuntionID(("ActorObject"),("Hello2"),GActorObject::Script_Hello2));

	return 0;
}
static int GActorObject_Index = Register_GActorObject();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
