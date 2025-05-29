

struct Light
{
	float3 LightIntensity;
	float  StartAttenuation;

	float3 LightDirection;
	float  EndAttenuation;

	float3 Position;
	int	   LightType;
};

float3 GetLightDirection(Light L, float3 InObjectWorldLocation)
{
	if (L.LightType == 0)
	{
		return L.LightDirection;
	}
	else if (L.LightType == 1)
	{
		return L.Position - InObjectWorldLocation;
	}

	return L.LightDirection;
}

float4 ComputeLightStrength(Light L,float3 InObjectPointNormal,float3 InObjectWorldLocation, float3 NormalizeLightDirection)
{
	//if (L.LightType == 0)
	//{

	//}
	/*else*/ 
	if (L.LightType == 1) //spot
	{
		float4 LightStrength = float4(1.f,1.f,1.f,1.f);
		
		float3 LightVector = L.Position - InObjectWorldLocation;;
		float Distance = length(LightVector);

		if (Distance < L.EndAttenuation)
		{
			float AttenuationRange = L.EndAttenuation - L.StartAttenuation;

			return LightStrength * (Distance / AttenuationRange);
		}
	}

	return float4( 0.f,0.f,0.f,1.f );
}

float4 Light_main() : SV_TARGET
{
    return 0;
}