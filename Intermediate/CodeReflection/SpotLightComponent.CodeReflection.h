#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CSpotLightComponent \
public: \
private:

#define CSpotLightComponent_9_Internal_Fun \
typedef CRangeLightComponent Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CSpotLightComponent_9_GENERATED_BODY_BT \
 Z_BT_CSpotLightComponent \
CSpotLightComponent_9_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CSpotLightComponent
#define CodeReflectionTagLine 9
