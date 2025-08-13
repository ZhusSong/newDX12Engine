// 2025.4.李
#pragma once
#include "Core/Engine.h"

class FEngineFactory
{
public:
	FEngineFactory();

	static CEngine* CreateEngine();
};