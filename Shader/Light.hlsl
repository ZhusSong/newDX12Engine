

struct Light
{
	float3 LightIntensity;
	float  xx;
	float3 LightDirection;
	float  xx1;
};


float4 Light_main() : SV_TARGET
{
    return 0;
}