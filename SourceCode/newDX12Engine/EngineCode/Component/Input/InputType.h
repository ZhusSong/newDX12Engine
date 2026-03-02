#pragma once
#include "../../EngineMinimal.h"

// 输入状态
// 入力状態
enum EPressState
{
	Press,   // 按下 //押下
	Release  // 松开 // 解放
};

// 当前按键值
// 現在のキー値
struct FInputKey
{
	FInputKey();

	string KeyName;
	EPressState PressState;

};