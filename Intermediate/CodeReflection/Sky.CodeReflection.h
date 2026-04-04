#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_GSky \
public: \
private:

#define GSky_10_Internal_Fun \
typedef GSphereMesh Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define GSky_10_GENERATED_BODY_BT \
 Z_BT_GSky \
GSky_10_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  GSky
#define CodeReflectionTagLine 10
