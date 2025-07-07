// 25.7.7 李
#pragma once
#include "../../../imgui/imgui.h"

class FEditorLogSystem
{
	friend class FLogEditor;
public:
	FEditorLogSystem();

	static FEditorLogSystem* Get();
	static void Destory();
public:
	void Clear();

	void AddLog(const char* Fmt, ...);
	void AddError(const char* Fmt, ...);
	void AddWarning(const char* Fmt, ...);
	void AddSuccess(const char* Fmt, ...);

protected:
	virtual void Draw(float DeltaTime);

protected:
	ImGuiTextBuffer TextBuff;
	ImGuiTextFilter TextFikter;
	ImVector<int>	LineOffsets;
	bool bAutoScroll;

protected:
	static FEditorLogSystem* LogSystem;
};