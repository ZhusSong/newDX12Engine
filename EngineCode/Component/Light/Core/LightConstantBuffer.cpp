// 灯光常量缓冲区
#include "LightConstantBuffer.h"

FLightConstantBuffer::FLightConstantBuffer()
{
	memset(&SceneLights, 0, sizeof(SceneLights));
}
