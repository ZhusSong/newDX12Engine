#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CFogComponent \
public: \
private:

#define CFogComponent_9_Internal_Fun \
typedef CComponent Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CFogComponent_9_GENERATED_BODY_BT \
 Z_BT_CFogComponent \
CFogComponent_9_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CFogComponent
#define CodeReflectionTagLine 9
