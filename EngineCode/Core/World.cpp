// 2025.4.李
#include "World.h"
#include "Camera.h"

CWorld::CWorld()
{
	Camera = CreateObject<GCamera>(new GCamera());
}
