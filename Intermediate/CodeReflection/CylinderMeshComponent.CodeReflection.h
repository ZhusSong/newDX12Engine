#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CCylinderMeshComponent \
public: \
private:

#define CCylinderMeshComponent_7_Internal_Fun \
typedef CShellMeshComponent Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CCylinderMeshComponent_7_GENERATED_BODY_BT \
 Z_BT_CCylinderMeshComponent \
CCylinderMeshComponent_7_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CCylinderMeshComponent
#define CodeReflectionTagLine 7
