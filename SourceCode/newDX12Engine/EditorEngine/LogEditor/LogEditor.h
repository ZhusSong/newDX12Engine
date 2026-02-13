#pragma once
#include "../Core/EditorBase.h"

// 日志
// ログ
class FLogEditor :public FEditorBase
{
public:
	virtual void BuildEditor();
	virtual void DrawEditor(float DeltaTime);
	virtual void ExitEditor();
};