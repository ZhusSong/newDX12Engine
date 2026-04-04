#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CEditorEngine \
public: \
private:

#define CEditorEngine_10_Internal_Fun \
typedef CEngine Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CEditorEngine_10_GENERATED_BODY_BT \
 Z_BT_CEditorEngine \
CEditorEngine_10_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CEditorEngine
#define CodeReflectionTagLine 10
