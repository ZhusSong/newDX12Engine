#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CPyramidMeshComponent \
public: \
private:

#define CPyramidMeshComponent_8_Internal_Fun \
typedef CShellMeshComponent Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CPyramidMeshComponent_8_GENERATED_BODY_BT \
 Z_BT_CPyramidMeshComponent \
CPyramidMeshComponent_8_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CPyramidMeshComponent
#define CodeReflectionTagLine 8
