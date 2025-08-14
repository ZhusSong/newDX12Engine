// 25.7.7 李
#pragma once
#include "../Core/EditorBase.h"

// 日志
class FLogEditor :public FEditorBase
{
public:
	virtual void BuildEditor();
	virtual void DrawEditor(float DeltaTime);
	virtual void ExitEditor();
};