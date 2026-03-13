#pragma once
#include "../../../Core/Engine.h"

// 所有渲染引擎的基类
// すべてのレンダリングエンジンの基底クラス
class CRenderingEngine : public CEngine
{
public:
	void SetMainWindowsHandle(HWND InNewMainWindowsHandle);

protected:
	HWND MainWindowsHandle;
};