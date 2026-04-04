#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CParallelLightComponent \
public: \
private:

#define CParallelLightComponent_9_Internal_Fun \
typedef CLightComponent Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CParallelLightComponent_9_GENERATED_BODY_BT \
 Z_BT_CParallelLightComponent \
CParallelLightComponent_9_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CParallelLightComponent
#define CodeReflectionTagLine 9
