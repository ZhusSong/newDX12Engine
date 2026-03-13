#pragma once
#include "../CoreObjectMinimal.h"

// 字节码表
// バイトコードテーブル
enum EScriptCommand
{
	Script_Undefined = 0x0B,   //默认的    // デフォルト
	Script_Int = 0x1D,         //int
	Script_Float = 0x1D,       //float
	Script_Funtion = 0x77,     //呼叫函数  // 関数呼び出し

	Script_Max = 0x100
};