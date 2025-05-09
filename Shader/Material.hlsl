
struct FMaterial
{
	//通用材质
    float4 BaseColor;
};

// 菲尼尔(边缘光)材质
// IntPowM 
float3 FresnelSchlickMethod(float3 InF0, float3 InObjectPointNormal, float3 InDirection, int InPowM)
{
    return InF0 + (1.0f - InF0) * pow((1.0f - saturate(dot(InObjectPointNormal, InDirection))), InPowM);

}

float4 Mat_main() : SV_TARGET
{
    return 0;
}