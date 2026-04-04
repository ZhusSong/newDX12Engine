#include "EngineRenderConfig.h"
#include "../EngineMinimal.h"
FEngineRenderConfig* FEngineRenderConfig::RenderConfig = nullptr;

FEngineRenderConfig::FEngineRenderConfig()
	:ScreenWidth(SCREEN_WIDTH)
	, ScreenHight(SCREEN_HEIGHT)
	, RefreshRate(60)
	, SwapChainCount(2)
{
}

FEngineRenderConfig* FEngineRenderConfig::GetRenderConfig()
{
	if (!RenderConfig)
	{
		RenderConfig = new FEngineRenderConfig();
	}

	return RenderConfig;
}

void FEngineRenderConfig::Destroy()
{
	if (RenderConfig)
	{
		delete RenderConfig;
		RenderConfig = nullptr;
	}
}
