#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CWindowsEngine \
public: \
private:

#define CWindowsEngine_14_Internal_Fun \
typedef CEngine Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CWindowsEngine_14_GENERATED_BODY_BT \
 Z_BT_CWindowsEngine \
CWindowsEngine_14_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CWindowsEngine
#define CodeReflectionTagLine 14
