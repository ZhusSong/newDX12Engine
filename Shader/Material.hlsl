
struct FMaterial
{
	//通用材质
    float4 BaseColor;
};

// 菲尼尔(边缘光)材质
// IntPowM 
float3 FresnelSchlickMethod(float3 InF0, float3 InObjectPointNormal, float3 InDirection, int InPowM)
{
    return InF0 + (1.f - InF0) * pow(1.f - saturate(dot(InObjectPointNormal, InDirection)), InPowM);
}

