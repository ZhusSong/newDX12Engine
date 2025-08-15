#pragma once
#include "../EngineMinimal.h"
#include "../EngineType.h"

class CWorld;
struct FCollisionSceneQuery
{
	// 射线检测
	static bool RaycastSingle(
		CWorld* InWorld,
		const XMVECTOR& OriginPoint,
		const XMVECTOR& Direction,
		const XMMATRIX& ViewInverseMatrix,
		FCollisionResult& OutResult);


	static bool RaycastSingle(
		CWorld* InWorld,
		GActorObject* InSpecificObjects,
		const XMVECTOR& OriginPoint,
		const XMVECTOR& Direction,
		const XMMATRIX& ViewInverseMatrix,
		FCollisionResult& OutResult);
};