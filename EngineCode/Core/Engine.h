﻿#pragma once
#include "CoreObject/CoreMinimalObject.h"

#if defined(_WIN32)
#include "WinMainCommandParameters.h"
#endif

// 引擎框架定义
class CEngine:public CCoreMinimalObject
{
public:
	CEngine();

	virtual int PreInit(
#if defined(_WIN32)
		FWinMainCommandParameters InParameters
#endif 
	) = 0;

	virtual int Init(
#if defined(_WIN32)
		FWinMainCommandParameters InParameters
#endif 
	) = 0;
	virtual int PostInit() = 0;

	virtual void Tick(float DeltaTime) = 0;

	virtual int PreExit() = 0;
	virtual int Exit() = 0;
	virtual int PostExit() = 0;
};