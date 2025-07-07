// 25.7.7 李
#pragma once
#include "../EngineCode/Core/Engine.h"
#include "Core/EditorBase.h"

// 编辑器引擎
class CEditorEngine
	:public CEngine
	, public FEditorBase
{
public:
	CEditorEngine();

	virtual int PreInit(
#if defined(_WIN32)
		FWinMainCommandParameters InParameters
#endif // 1
	);

	virtual int Init(
#if defined(_WIN32)
		FWinMainCommandParameters InParameters
#endif // 1
	);
	virtual int PostInit();

	virtual void Tick(float DeltaTime) {};

	virtual int PreExit();
	virtual int Exit();
	virtual int PostExit();

public:
	virtual void BuildEditor();
	virtual void DrawEditor(float DeltaTime);
	virtual void ExitEditor();

public:
	void DrawLayer(float DeltaTime);

protected:
	class FToolbarEditor* ToolbarEditor;
	class FLogEditor* LogEditor;
};