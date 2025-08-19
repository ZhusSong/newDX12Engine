#pragma once
#include "../EngineCode/EngineMinimal.h"

#if EDITOR_ENGINE
class GActorObject;

class FOperationHandleSelectManager
{
public:
	FOperationHandleSelectManager();

	static FOperationHandleSelectManager* Get();
	static void Destroy();

public:
	void AllOperationHandleHide();

	// 显示选中的手柄
	void DisplaySelectedOperationHandle(GActorObject* InNewSelectedObject);
	void DisplaySelectedOperationHandle();
	// 隐藏选中的手柄
	void HideSelectedOperationHandle();

public:
	// 显示新的手柄
	void SetNewSelectedOperationHandle(GActorObject* InNewSelectedObject);
	// 设置选择对象
	void SetNewSelectedObject(GActorObject* InNewSelectedObject);

public:
	GActorObject* GetSelectedOperationHandle();
private:
	static FOperationHandleSelectManager* InManage;

	// 记录当前的操作手柄 方便下次再显示出来
	GActorObject* SelectedOperationHandle;
};

#endif