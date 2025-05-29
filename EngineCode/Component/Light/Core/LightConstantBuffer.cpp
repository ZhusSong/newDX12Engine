// 灯光常量缓冲区
#include "LightConstantBuffer.h"
#include "LightType.h"

FLightConstantBuffer::FLightConstantBuffer()
{
	memset(&SceneLights, 0, sizeof(SceneLights));
}

FLight::FLight()
	:StartAttenuation(1.f)
	, EndAttenuation(10.f)
	, LightType((int)ELightType::DirectionalLight)
{
}
