#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_GPlaneMesh \
public: \
private:

#define GPlaneMesh_8_Internal_Fun \
typedef GMesh Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define GPlaneMesh_8_GENERATED_BODY_BT \
 Z_BT_GPlaneMesh \
GPlaneMesh_8_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  GPlaneMesh
#define CodeReflectionTagLine 8
