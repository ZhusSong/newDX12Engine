#pragma once

#if defined(_WIN32)
#include "../EngineMinimal.h"

// 定义Windows环境下所需参数
class FWinMainCommandParameters
{
public:
	FWinMainCommandParameters(HINSTANCE InhInstance, HINSTANCE InprevInstance, PSTR IncmdLine, int InshowCmd);

	HINSTANCE HInstance;
	HINSTANCE PrevInstance;
	PSTR CmdLine;
	int ShowCmd;
};

#elif 0

#endif // 0

