#pragma once
#include "../EngineMinimal.h"
#include "../EngineType.h"

class CWorld;
struct FRaycastSystemLibrary
{
	// 判断射线检测结果
	static bool HitResultByScreen(CWorld* InWorld, int ScreenX, int ScreenY, FCollisionResult& OutResult);
};