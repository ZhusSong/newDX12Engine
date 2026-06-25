#ifndef MATERIAL_HLSL
#define MATERIAL_HLSL

#include "ShaderCommon.hlsli"
#include "ShaderFunctionLibrary.hlsli"

// 通用材质数据
// 汎用マテリアルデータ
struct FMaterial
{
    float4 BaseColor;
};

// Schlick 近似菲涅尔
// Schlick近似フレネル
float3 FresnelSchlickMethod(float3 InF0, float3 InObjectPointNormal, float3 InDirection, int InPowM)
{
    return InF0 + (1.f - InF0) * pow(1.f - saturate(dot(InObjectPointNormal, InDirection)), InPowM);
}

// 获取材质基础颜色
// マテリアルのベースカラーを取得
float4 GetMaterialBaseColor(MaterialConstBuffer MatConstBuffer, float2 InTexCoord)
{
    if (MatConstBuffer.BaseColorIndex != -1)
    {
        return SimpleTexture2DMap[MatConstBuffer.BaseColorIndex].Sample(TextureSampler, InTexCoord);
    }

    return MatConstBuffer.BaseColor;
}

// 获取材质法线
// マテリアル法線を取得
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

        float3x3 TBN = GetBuildTBNMatrix(InUnitWorldNormal, InWorldTangent);

        return mul(NormalsInTangentSpace, TBN);
    }

    return InUnitWorldNormal;
}

// 获取高光颜色
// スペキュラ色を取得
float4 GetMaterialSpecular(MaterialConstBuffer MatConstBuffer, float2 InTexCoord)
{
    if (MatConstBuffer.SpecularIndex != -1)
    {
        return SimpleTexture2DMap[MatConstBuffer.SpecularIndex].Sample(TextureSampler, InTexCoord);
    }

    return float4(MatConstBuffer.SpecularColor, 1.f);
}

// 获取正交化切线
// 直交化された接線を取得
float3 GetOrthonormalizedTangent(float3 InWorldTangent, float3 InUnitWorldNormal)
{
    return normalize(InWorldTangent - dot(InWorldTangent, InUnitWorldNormal) * InUnitWorldNormal);
}

// 偏移各向异性方向
// 異方性方向をシフト
float3 ShiftAnisotropyDirection(float3 InUnitWorldTangent, float3 InUnitWorldNormal, float InShift)
{
    return normalize(InUnitWorldTangent + InShift * InUnitWorldNormal);
}

// 获取 Kajiya-Kay 漫反射项
// Kajiya-Kay 拡散項を取得
float GetKajiyaKayDiffuse(float3 InUnitWorldTangent, float3 InUnitLightDirection)
{
    float TangentLight = dot(InUnitWorldTangent, InUnitLightDirection);
    return sqrt(saturate(1.0f - TangentLight * TangentLight));
}

// 获取 Kajiya-Kay 高光项
// Kajiya-Kay スペキュラ項を取得
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

// 兼容旧折射参数与IOR折射率输入
// 旧来の屈折パラメータとIOR入力を両対応する
float GetRefractionEta(float InRefractiveValue)
{
    if (InRefractiveValue <= 0.0f)
    {
        return 1.0f;
    }

    if (InRefractiveValue < 1.0f)
    {
        return InRefractiveValue;
    }

    return 1.0f / InRefractiveValue;
}

// 获取折射方向
// 屈折方向を取得
float3 GetRefract(float3 InUnitWorldNormal, float3 WorldPosition, float InRefractiveValue)
{
    float3 ViewDirection = normalize(ViewportPosition.xyz - WorldPosition);
    return refract(-ViewDirection, InUnitWorldNormal, GetRefractionEta(InRefractiveValue));
}

// 获取反射采样颜色
// 反射サンプリング色を取得
float3 GetReflectionSampleColor(float3 InUnitWorldNormal, float3 NewReflect)
{
    return SimpleCubeMap.Sample(TextureSampler, NewReflect).rgb;
}

// 获取平面反射采样颜色
// 平面反射サンプリング色を取得
float3 GetPlanarReflectionSampleColor(float3 InUnitWorldNormal, float3 WorldPosition)
{
    if (PlanarReflectionSettings.x <= 0.5f || PlanarReflectionSettings.y < 0.0f)
    {
        return float3(0.f, 0.f, 0.f);
    }

    float3 ViewDirection = normalize(ViewportPosition.xyz - WorldPosition);
    if (dot(normalize(InUnitWorldNormal), ViewDirection) <= 0.0f)
    {
        return float3(0.f, 0.f, 0.f);
    }

    float4 PlanarPosition = mul(float4(WorldPosition, 1.f), PlanarReflectionTexViewProjection);
    if (PlanarPosition.w <= 0.0001f)
    {
        return float3(0.f, 0.f, 0.f);
    }

    PlanarPosition /= PlanarPosition.w;

    PlanarPosition.xy = saturate(PlanarPosition.xy);

    int TextureIndex = (int)(PlanarReflectionSettings.y + 0.5f);
    return SimpleTexture2DMap[TextureIndex].Sample(TextureSampler, PlanarPosition.xy).rgb;
}

// 根据材质设置选择反射采样源
// マテリアル設定に応じて反射サンプル元を選択
float3 GetReflectionSampleColor(MaterialConstBuffer MatConstBuffer, float3 InUnitWorldNormal, float3 WorldPosition, float3 NewReflect)
{
    if (MatConstBuffer.UsePlanarReflection > 0.5f)
    {
        return GetPlanarReflectionSampleColor(InUnitWorldNormal, WorldPosition);
    }

    return GetReflectionSampleColor(InUnitWorldNormal, NewReflect);
}

// 得到反射强度(光泽度)
// 反射強度（光沢度）を取得
float GetShininess(MaterialConstBuffer MatConstBuffer)
{
    return 1.f - MatConstBuffer.MaterialRoughness;
}

// 获取菲涅尔反射系数
// フレネル反射係数を取得
float3 FresnelSchlickFactor(MaterialConstBuffer MatConstBuffer, float3 InUnitWorldNormal, float3 InReflect)
{
    return FresnelSchlickMethod(MatConstBuffer.FresnelF0, InUnitWorldNormal, InReflect, 5);
}

// 获取玻璃透明度
// ガラス透明度を取得
float GetGlassOpacity(MaterialConstBuffer MatConstBuffer, float3 InUnitWorldNormal, float3 WorldPosition)
{
    float3 ViewDirection = normalize(ViewportPosition.xyz - WorldPosition);
    float3 FresnelFactor = FresnelSchlickFactor(MatConstBuffer, InUnitWorldNormal, ViewDirection);
    float FresnelMax = max(max(FresnelFactor.x, FresnelFactor.y), FresnelFactor.z);

    return saturate(MatConstBuffer.Transparency * 0.2f + FresnelMax * 0.8f);
}

// 粗糙度版本的菲涅尔近似
// 粗さ対応のフレネル近似
float3 FresnelSchlickRoughness(float NV, float3 F0, float Roughness)
{
    return F0 + (max(float3(1.0 - Roughness, 1.0 - Roughness, 1.0 - Roughness), F0) - F0) * pow(1.0 - NV, 5.0);
}

// 获取最终反射颜色
// 最終反射色を取得
float3 GetReflectionColor(MaterialConstBuffer MatConstBuffer, float3 InUnitWorldNormal, float3 WorldPosition)
{
    float3 NewReflect = GetReflect(InUnitWorldNormal, WorldPosition);
    float3 SampleReflectionColor = GetReflectionSampleColor(MatConstBuffer, InUnitWorldNormal, WorldPosition, NewReflect);
    float Shininess = GetShininess(MatConstBuffer);
    float3 FresnelFactor = FresnelSchlickFactor(MatConstBuffer, InUnitWorldNormal, NewReflect);

    return SampleReflectionColor * FresnelFactor * Shininess;
}

// 获取折射颜色
// 屈折色を取得
float3 GetRefractColor(MaterialConstBuffer MatConstBuffer, float InRefractiveIndex, float3 InUnitWorldNormal, float3 WorldPosition)
{
    float3 NewRefract = GetRefract(InUnitWorldNormal, WorldPosition, MatConstBuffer.Refraction);
    float3 SampleReflectionColor = GetReflectionSampleColor(MatConstBuffer, InUnitWorldNormal, WorldPosition, NewRefract);
    float Shininess = GetShininess(MatConstBuffer);
    float3 FresnelFactor = FresnelSchlickFactor(MatConstBuffer, InUnitWorldNormal, NewRefract);

    return SampleReflectionColor * FresnelFactor * Shininess;
}
#endif
