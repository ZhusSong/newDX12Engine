#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CPointLightComponent \
public: \
private:

#define CPointLightComponent_9_Internal_Fun \
typedef CRangeLightComponent Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CPointLightComponent_9_GENERATED_BODY_BT \
 Z_BT_CPointLightComponent \
CPointLightComponent_9_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CPointLightComponent
#define CodeReflectionTagLine 9
