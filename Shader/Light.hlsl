

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
	    // 取反，保证方向与模型方向一致
		return -L.LightDirection;
	}
	else if (L.LightType == 1)
	{
		return L.Position - InObjectWorldLocation;
	}
	else if (L.LightType == 2)
	{
		return L.Position - InObjectWorldLocation;
	}

	return L.LightDirection;
}

// 计算距离衰减
float4 AttenuationPointLights1(Light L,float Distance)
{
	float4 LightStrength = float4(1.f, 1.f, 1.f, 1.f);

	float AttenuationRange = L.EndAttenuation - L.StartAttenuation;

	return LightStrength * (Distance / AttenuationRange);
}

// 计算根据衰减因子的衰减
float4 AttenuationPointLights2(Light L,float Distance,float C,float I,float Q)
{
	float4 LightStrength = float4(1.f, 1.f, 1.f, 1.f);
	return (1.f / (C + I * Distance + pow(Q, 2.f) * Distance)) * LightStrength;
}

float4 ComputeLightStrength(Light L,float3 InObjectPointNormal,float3 InObjectWorldLocation, float3 NormalizeLightDirection)
{
    // 平行光时
	if (L.LightType == 0)
	{
		return float4(1.f, 1.f, 1.f, 1.f);
	}
	// Point Light 
	else if (L.LightType == 1) //Point
	{
		float4 LightStrength = float4(1.f, 1.f, 1.f, 1.f);
		float3 LightVector = L.Position - InObjectWorldLocation;;
		float Distance = length(LightVector);

		if (Distance < L.EndAttenuation)
		{
			return AttenuationPointLights1(L, Distance) * LightStrength;
			//return AttenuationPointLights2(
			//	L,
			//	Distance,
			//	0.f,//c
			//	0.5f,//i
			//	0.9f);//q
		}
	}
	else if (L.LightType == 2) //spot
	{
		float3 LightVector = L.Position - InObjectWorldLocation;;
		float Distance = length(LightVector);

		if (Distance < L.EndAttenuation)
		{	
			float4 LightStrength = float4(1.f, 1.f, 1.f, 1.f) * pow(max(dot(NormalizeLightDirection, L.LightDirection), 0.f),1.f);

			//return AttenuationPointLights1(L, Distance) * LightStrength;
			return AttenuationPointLights2(
				L,
				Distance,
				0.f,//c
				0.4f,//i
				0.3f)* LightStrength;//q
		}
	}

	return float4( 0.f,0.f,0.f,1.f );
}

float4 Light_main() : SV_TARGET
{
    return 0;
}