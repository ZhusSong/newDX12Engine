#pragma once
// 添加附加库头文件与链接库
// 追加ライブラリのヘッダーファイルとリンクライブラリを追加
#include "simple_library/public/simple_library.h"
#pragma comment(lib, "EngineSimpleLibrary.lib")

// 链接EngineCore
// EngineCoreにリンク
#include "Core/Path/EnginePath.h"
#pragma comment(lib, "DX12EngineGeneralSetting.lib")