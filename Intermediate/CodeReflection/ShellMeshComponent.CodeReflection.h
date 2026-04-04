#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CShellMeshComponent \
public: \
private:

#define CShellMeshComponent_10_Internal_Fun \
typedef CMeshComponent Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CShellMeshComponent_10_GENERATED_BODY_BT \
 Z_BT_CShellMeshComponent \
CShellMeshComponent_10_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CShellMeshComponent
#define CodeReflectionTagLine 10
