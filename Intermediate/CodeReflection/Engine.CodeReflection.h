#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CEngine \
public: \
private:

#define CEngine_14_Internal_Fun \
typedef CCoreMinimalObject Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CEngine_14_GENERATED_BODY_BT \
 Z_BT_CEngine \
CEngine_14_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CEngine
#define CodeReflectionTagLine 14
