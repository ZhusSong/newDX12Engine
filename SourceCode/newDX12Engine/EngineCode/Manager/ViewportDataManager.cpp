#include "ViewportDataManager.h"
#include "../Config/EngineRenderConfig.h"

FViewportDataManager::FViewportDataManager()
	:Width(FEngineRenderConfig::GetRenderConfig()->ScreenWidth)
	, Height(FEngineRenderConfig::GetRenderConfig()->ScreenHight)
	, YFOV(0.25f * XM_PI)
	, Aspect((float)Width / (float)Height)
	, ZNear(0.1f)
	, ZFar(10000.f)
{

}

void FViewportDataManager::ResetSize(int InWidth, int InHeight)
{
	Width = InWidth;
	Height = InHeight;

	Aspect = (float)InWidth / (float)InHeight;
}
