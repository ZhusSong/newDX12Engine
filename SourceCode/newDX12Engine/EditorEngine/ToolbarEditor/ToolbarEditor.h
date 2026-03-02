#pragma once
#include "../Core/EditorBase.h"

class FToolbarEditor :public FEditorBase
{
public:
	virtual void BuildEditor();
	virtual void DrawEditor(float DeltaTime);
	virtual void ExitEditor();

public:
	void NewLevel();
};