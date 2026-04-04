#pragma once

#include "CoreObject/CoreMinimalObject.h"
#include "CodeReflection/Frame.h"
#include "CodeReflection/ScriptMacro.h"

#define  Z_BT_CDirectX12RenderingEngine \
public: \
private:

#define CDirectX12RenderingEngine_8_Internal_Fun \
typedef CDirectXRenderingEngine Super; \
protected: \
virtual void InitReflectionContent(); \
private: 

#define CDirectX12RenderingEngine_8_GENERATED_BODY_BT \
 Z_BT_CDirectX12RenderingEngine \
CDirectX12RenderingEngine_8_Internal_Fun 

#ifdef CURRENT_FILE_ID_BT
#undef CURRENT_FILE_ID_BT
#endif // CURRENT_FILE_ID_BT

#ifdef CodeReflectionTagLine
#undef CodeReflectionTagLine
#endif // NewLine

#define CURRENT_FILE_ID_BT  CDirectX12RenderingEngine
#define CodeReflectionTagLine 8
