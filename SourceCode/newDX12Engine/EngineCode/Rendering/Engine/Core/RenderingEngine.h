#pragma once
#include "../../../Core/Engine.h"
#include "RenderingEngine.CodeReflection.h"

// 所有渲染引擎的基类
// すべてのレンダリングエンジンの基底クラス
class CRenderingEngine : public CEngine
{
	CODEREFLECTION()
public:
	void SetMainWindowsHandle(HWND InNewMainWindowsHandle);

protected:
	HWND MainWindowsHandle;
};