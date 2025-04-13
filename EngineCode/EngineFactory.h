#pragma once
#include "Frame/Engine.h"

class FEngineFactory
{
public:
	FEngineFactory();

	static FEngine* CreateEngine();
};