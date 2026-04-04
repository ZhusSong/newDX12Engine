#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_GFog \
public: \
private:

#define GFog_10_Internal_Fun \
typedef GActorObject Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define GFog_10_GENERATED_BODY_BT \
 Z_BT_GFog \
GFog_10_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  GFog
#define CodeReflectionTagLine 10
