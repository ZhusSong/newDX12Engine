#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_GCamera \
public: \
private:

#define GCamera_18_Internal_Fun \
typedef GClientViewport Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define GCamera_18_GENERATED_BODY_BT \
 Z_BT_GCamera \
GCamera_18_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  GCamera
#define CodeReflectionTagLine 18
