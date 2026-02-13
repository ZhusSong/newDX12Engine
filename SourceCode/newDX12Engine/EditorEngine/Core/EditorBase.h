#pragma once
#include "../../EngineCode/EngineMinimal.h"

// 编辑器母类
// エディタの基底クラス
class FEditorBase
{
public:
	// 创建
	// 作成
	virtual void BuildEditor() = 0;
	// 绘制
	// 描画
	virtual void DrawEditor(float DeltaTime) = 0;
	// 销毁
	// 破棄
	virtual void ExitEditor() = 0;
};
