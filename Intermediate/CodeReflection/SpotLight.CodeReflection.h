#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_GSpotLight \
public: \
private:

#define GSpotLight_8_Internal_Fun \
typedef GRangeLight Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define GSpotLight_8_GENERATED_BODY_BT \
 Z_BT_GSpotLight \
GSpotLight_8_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  GSpotLight
#define CodeReflectionTagLine 8
