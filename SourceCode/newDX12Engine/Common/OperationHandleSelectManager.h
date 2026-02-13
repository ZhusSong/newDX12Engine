#pragma once
#include "../EngineCode/EngineMinimal.h"

#if EDITOR_ENGINE
class GActorObject;

// 可操作手柄管理
// 操作可能なハンドルの管理クラス
class FOperationHandleSelectManager
{
public:
	FOperationHandleSelectManager();

	static FOperationHandleSelectManager* Get();
	static void Destroy();

public:
	void AllOperationHandleHide();

	// 显示选中的手柄
	// 選択されたハンドルを表示する
	void DisplaySelectedOperationHandle(GActorObject* InNewSelectedObject);
	void DisplaySelectedOperationHandle();
	// 隐藏选中的手柄
	// 選択されたハンドルを非表示にする
	void HideSelectedOperationHandle();

public:
	// 显示新的手柄
	// 新しいハンドルを表示する
	void SetNewSelectedOperationHandle(GActorObject* InNewSelectedObject);
	// 设置选择对象
	// 選択オブジェクトを設定する
	void SetNewSelectedObject(GActorObject* InNewSelectedObject);

	void SetHitObject(bool isHit);

public:
	GActorObject* GetSelectedOperationHandle();
private:
	static FOperationHandleSelectManager* InManage;

	// 记录当前的操作手柄 方便下次再显示出来
	// 現在の操作ハンドルを記録し、次回再表示できるようにする
	GActorObject* SelectedOperationHandle;
};

#endif
