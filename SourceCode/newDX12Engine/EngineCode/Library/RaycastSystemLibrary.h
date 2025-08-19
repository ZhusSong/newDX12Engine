#pragma once
#include "../EngineMinimal.h"
#include "../EngineType.h"

class CWorld;
struct FRaycastSystemLibrary
{
	// 判断射线检测结果
	static bool HitResultByScreen(CWorld* InWorld, int ScreenX, int ScreenY, FCollisionResult& OutResult);
	
	// 判断射线是否点击到物体
	static bool HitSpecificObjectsResultByScreen(
		CWorld* InWorld, GActorObject* InSpecificObjects,
		const std::vector<CComponent*>& IgnoreComponents,
		int ScreenX, int ScreenY, FCollisionResult& OutResult);

	// 从屏幕坐标到世界空间的转换
	static bool GetRaycastByScreenParam(
		CWorld* InWorld,
		const fvector_2id& ScreenXY,
		XMVECTOR& OriginPoint,
		XMVECTOR& Direction,
		XMMATRIX& ViewInverseMatrix);

};