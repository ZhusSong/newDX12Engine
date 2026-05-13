#ifndef MATERIAL_HLSL
#define MATERIAL_HLSL

#include "ShaderCommon.hlsli"
#include "ShaderFunctionLibrary.hlsli"

struct FMaterial
{
	// 通用材质
    // 汎用マテリアル
    float4 BaseColor;
};

float3 FresnelSchlickMethod(float3 InF0, float3 InObjectPointNormal, float3 InDirection, int InPowM)
{
    return InF0 + (1.f - InF0) * pow(1.f - saturate(dot(InObjectPointNormal, InDirection)), InPowM);
}

float4 GetMaterialBaseColor(MaterialConstBuffer MatConstBuffer, float2 InTexCoord)
{
    if (MatConstBuffer.BaseColorIndex != -1)
    {
        return SimpleTexture2DMap[MatConstBuffer.BaseColorIndex].Sample(TextureSampler, InTexCoord);
    }

    return MatConstBuffer.BaseColor;
}

float3 GetMaterialNormals(
	MaterialConstBuffer MatConstBuffer,
	float2 InTexCoord,
	float3 InUnitWorldNormal,
	float3 InWorldTangent)
{
    if (MatConstBuffer.NormalIndex != -1)
    {
        float4 SampleNormal = SimpleTexture2DMap[MatConstBuffer.NormalIndex].Sample(AnisotropicSampler, InTexCoord);
	
		//[0,1]->[-1,1] => [0,1] * 2.f = [0,2] => [0-2]-1.f = [-1,1];
        float3 NormalsInTangentSpace = 2.0f * SampleNormal.rgb - 1.f;

		// 拿到世界TBN
        // ワールドTBNを取得
        float3x3 TBN = GetBuildTBNMatrix(InUnitWorldNormal, InWorldTangent);

		// 把切线空间下的采样法线转为世界的法线
        // タンジェント空間でサンプリングされた法線をワールド法線に変換
        return mul(NormalsInTangentSpace, TBN);
    }

    return InUnitWorldNormal;
}

float4 GetMaterialSpecular(MaterialConstBuffer MatConstBuffer, float2 InTexCoord)
{
    if (MatConstBuffer.SpecularIndex != -1)
    {
        return SimpleTexture2DMap[MatConstBuffer.SpecularIndex].Sample(TextureSampler, InTexCoord);
    }

    return float4(MatConstBuffer.SpecularColor, 1.f);
}

float3 GetOrthonormalizedTangent(float3 InWorldTangent, float3 InUnitWorldNormal)
{
    return normalize(InWorldTangent - dot(InWorldTangent, InUnitWorldNormal) * InUnitWorldNormal);
}

float3 ShiftAnisotropyDirection(float3 InUnitWorldTangent, float3 InUnitWorldNormal, float InShift)
{
    return normalize(InUnitWorldTangent + InShift * InUnitWorldNormal);
}

float GetKajiyaKayDiffuse(float3 InUnitWorldTangent, float3 InUnitLightDirection)
{
    float TangentLight = dot(InUnitWorldTangent, InUnitLightDirection);
    return sqrt(saturate(1.0f - TangentLight * TangentLight));
}

float GetKajiyaKaySpecular(float3 InUnitWorldTangent, float3 InHalfDirection, float InExponent)
{
    float TangentHalf = dot(InUnitWorldTangent, InHalfDirection);
    return pow(sqrt(saturate(1.0f - TangentHalf * TangentHalf)), max(InExponent, 1.0f));
}

// 获取反射方向
// 反射方向を取得
float3 GetReflect(float3 InUnitWorldNormal, float3 WorldPosition)
{
    float3 ViewDirection = normalize(ViewportPosition.xyz - WorldPosition);
    return reflect(-ViewDirection, InUnitWorldNormal);
}
// 获取折射
// 屈折を取得
float3 GetRefract(float3 InUnitWorldNormal, float3 WorldPosition, float InRefractiveValue)
{
    float3 ViewDirection = normalize(ViewportPosition.xyz - WorldPosition);
    return refract(-ViewDirection, InUnitWorldNormal, InRefractiveValue);
}


// 获取反射采样
// 反射サンプリングを取得
float3 GetReflectionSampleColor(float3 InUnitWorldNormal, float3 NewReflect)
{
    return SimpleCubeMap.Sample(TextureSampler, NewReflect);
}

// 得到反射强度(光泽度)
// 反射強度（光沢度）を取得
float GetShininess(MaterialConstBuffer MatConstBuffer)
{
    return 1.f - MatConstBuffer.MaterialRoughness;
}

// 获取菲尼尔参数
// フレネルパラメータを取得
float3 FresnelSchlickFactor(MaterialConstBuffer MatConstBuffer, float3 InUnitWorldNormal, float3 InReflect)
{
    return FresnelSchlickMethod(MatConstBuffer.FresnelF0, InUnitWorldNormal, InReflect, 5);
}
float3 FresnelSchlickRoughness(float NV, float3 F0, float Roughness)
{
    return F0 + (max(float3(1.0 - Roughness, 1.0 - Roughness, 1.0 - Roughness), F0) - F0) * pow(1.0 - NV, 5.0);
}
// 得到最终反射颜色
// 最終反射色を取得
float3 GetReflectionColor(MaterialConstBuffer MatConstBuffer, float3 InUnitWorldNormal, float3 WorldPosition)
{
    float3 NewReflect = GetReflect(InUnitWorldNormal, WorldPosition);
    float3 SampleReflectionColor = GetReflectionSampleColor(InUnitWorldNormal, NewReflect);
    float Shininess = GetShininess(MatConstBuffer);
    float3 FresnelFactor = FresnelSchlickFactor(MatConstBuffer, InUnitWorldNormal, NewReflect);

    return SampleReflectionColor * FresnelFactor * Shininess;
}

// 获取折射的颜色 
// 屈折の色を取得
float3 GetRefractColor(MaterialConstBuffer MatConstBuffer, float InRefractiveIndex, float3 InUnitWorldNormal, float3 WorldPosition)
{
    float3 NewRefract = GetRefract(InUnitWorldNormal, WorldPosition, MatConstBuffer.Refraction);
    float3 SampleReflectionColor = GetReflectionSampleColor(InUnitWorldNormal, NewRefract);
    float Shininess = GetShininess(MatConstBuffer);
    float3 FresnelFactor = FresnelSchlickFactor(MatConstBuffer, InUnitWorldNormal, NewRefract);

    return SampleReflectionColor * FresnelFactor * Shininess;
}
#endif
