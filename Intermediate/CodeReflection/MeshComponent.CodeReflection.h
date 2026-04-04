#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CMeshComponent \
public: \
private:

#define CMeshComponent_12_Internal_Fun \
typedef CTransformComponent Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CMeshComponent_12_GENERATED_BODY_BT \
 Z_BT_CMeshComponent \
CMeshComponent_12_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CMeshComponent
#define CodeReflectionTagLine 12
