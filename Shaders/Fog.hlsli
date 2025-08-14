//25.6.16 李
#ifndef Fog
#define Fog

#include "ShaderCommon.hlsli"

float4 GetFogValue(float4 InColor,float3 InPointPosition)
{
#if START_UP_FOG
	float Distance = length(InPointPosition - ViewportPosition.xyz);

	float AlphaValue = saturate((Distance - FogStart) / FogRange);

	float3 Color = lerp(InColor.xyz, FogColor.xyz, AlphaValue);

	float HeightAlphaValue = saturate((InPointPosition.y - ViewportPosition.y) / max(FogHeight,1.f));

	InColor.xyz = lerp(Color.xyz, InColor.xyz,HeightAlphaValue - FogTransparentCoefficient);
#endif


	return InColor;
}
#endif