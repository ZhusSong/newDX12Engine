#pragma once
#include "../../../imgui/imgui.h"
#include "simple_library/public/simple_library.h"

// 日志对象
// ログ管理クラス
class FEditorLogSystem
{
	friend class FLogEditor;
public:
	FEditorLogSystem();

	static FEditorLogSystem* Get();
	static void Destroy();

public:
	void Clear();

	void AddLog(const char* Fmt, ...);
	void AddError(const char* Fmt, ...);
	void AddWarning(const char* Fmt, ...);
	void AddSuccess(const char* Fmt, ...);

protected:
	virtual void Draw(float DeltaTime);

	// 添加每一行的颜色状态
	// 各行のカラー状態を追加する
	void AddLineColor(e_error InColor);

	// 重置行偏移
	// 行オフセットをリセットする
	void ResetLineOffsets(e_error InColor, int InOldSize);

	// 获取e_error对应的颜色数据
	// e_errorに対応するカラー情報を取得する
	ImVec4 GetColor(e_error InColorID);

	void HandleBackstageLog(e_error InColorID, int InOldSize);

protected: 
	// 字体buff 里面存储字体 用/n来区分
	// フォントバッファ。文字列を格納し、\nで区切る
	ImGuiTextBuffer TextBuff;    

	// 字体过滤器 用于查询
	// テキストフィルター。ログ検索用
	ImGuiTextFilter TextFilter;    

	// 每一段字体的偏移，用 / n来区分
	// 各行の開始位置オフセット（\nで区切る）
	ImVector<int> LineOffsets;     
	
	// 每一行的颜色
	// 各行のカラー情報
	ImVector<e_error> PreLineColor; 

	// 自动更新到最新日志
	// 最新ログへ自動スクロールするかどうか
	bool bAutoScroll;              

protected:
	static FEditorLogSystem* LogSystem;
};
