#pragma once
#include "../../EngineMinimal.h"

// 按下状态
enum EPressState
{
	Press,  //按下
	Release //松开
};
// 当前按键值
struct FInputKey
{
	FInputKey();

	string KeyName;
	EPressState PressState;

};