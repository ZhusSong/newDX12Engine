// 2025.4.李
#include "World.h"
#include "Camera.h"

CWorld::CWorld()
{
	Camera = CreateObject<GCamera>(new GCamera());
}

bool CWorld::LineTraceBySingle(FCollisionResult& OutResult, const fvector_3d& InStart, const fvector_3d& InEnd)
{
	return false;
}
