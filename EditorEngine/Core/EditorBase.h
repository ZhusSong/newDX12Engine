// 25.7.7 李
#pragma once
#include "../../EngineCode/EngineMinimal.h"

// 编辑器母类
class FEditorBase
{
public:
	// 创建
	virtual void BuildEditor() = 0;
	// 绘制
	virtual void DrawEditor(float DeltaTime) = 0;
	// 销毁
	virtual void ExitEditor() = 0;
};