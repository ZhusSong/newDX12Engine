#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_GActorObject \
FUNCTION_DEFINITION(Script_Hello1) \
{ \
	GActorObject::Hello1(); \
} \
FUNCTION_DEFINITION(Script_Hello2) \
{ \
	GActorObject::Hello2(); \
} 

#define GActorObject_12_GENERATED_BODY_BT \
 Z_BT_GActorObject

#define CURRENT_FILE_ID_BT  GActorObject
#define NewLine 12
