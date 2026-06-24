#ifndef SHADOWFUNCTION_HLSL
#define SHADOWFUNCTION_HLSL

#include "ShaderCommon.hlsli"

// 直接采样 有锯齿
// 直接サンプリング ジャギーあり
float GetShadowFactor(float4 InWorldPosition, float4x4 InShadowMatrix)
{
    float4 ShadowPointHome = mul(InWorldPosition, InShadowMatrix);

    float ShadowDepth = ShadowPointHome.z;
    return SimpleShadowMap.SampleCmpLevelZero(ShadowSampler, ShadowPointHome.xy, ShadowDepth);
}

// 直接采样 有锯齿 有透视
// 直接サンプリング ジャギーあり 透視あり
float GetShadowFactorByDirectSample(float4 InWorldPosition, float4x4 InShadowMatrix)
{
    float4 ShadowPointHome = mul(InWorldPosition, InShadowMatrix);
    return SimpleShadowMap.Sample(TextureSampler, ShadowPointHome.xy).r;
}

// 四个样本PCF采样 有模糊的锯齿
// 4サンプルPCFサンプリング ぼやけたジャギーあり
float GetShadowFactor_PCF_Sample4(float4 InWorldPosition, float4x4 InShadowMatrix)
{
    uint Width = 0;
    uint Height = 0;
    uint NumMips = 0;

    SimpleShadowMap.GetDimensions(0, Width, Height, NumMips);

    float SampleSize = 1.f / Width;

    float4 ShadowPointHome = mul(InWorldPosition, InShadowMatrix);
    float S1 = SimpleShadowMap.Sample(TextureSampler, ShadowPointHome.xy).r;
    float S2 = SimpleShadowMap.Sample(TextureSampler, ShadowPointHome.xy + float2(SampleSize, 0.f)).r;
    float S3 = SimpleShadowMap.Sample(TextureSampler, ShadowPointHome.xy + float2(0.f, SampleSize)).r;
    float S4 = SimpleShadowMap.Sample(TextureSampler, ShadowPointHome.xy + float2(SampleSize, SampleSize)).r;

    ShadowPointHome.xyz /= ShadowPointHome.w;

    float ShadowDepth = ShadowPointHome.z;

    float R1 = ShadowDepth <= S1;
    float R2 = ShadowDepth <= S2;
    float R3 = ShadowDepth <= S3;
    float R4 = ShadowDepth <= S4;

	// 转到纹素空间
    // テクセル空間に変換
    float2 TexelsPosition = frac(Width * ShadowPointHome.xy);

    return lerp(
		lerp(R1, R2, TexelsPosition.x),
		lerp(R3, R4, TexelsPosition.x),
		TexelsPosition.y);
}

// 九个点的PCF采样
// 9点のPCFサンプリング
float GetShadowFactor_PCF_Sample9(float4 InWorldPosition, float4x4 InShadowMatrix)
{
    float4 ShadowPointHome = mul(InWorldPosition, InShadowMatrix);

    ShadowPointHome.xyz /= ShadowPointHome.w;
    float ShadowDepth = ShadowPointHome.z;

    uint Width = 0;
    uint Height = 0;
    uint NumMips = 0;

    SimpleShadowMap.GetDimensions(0, Width, Height, NumMips);
    float SampleSize = 1.f / Width;

    const float2 ConstShadowSample[9] =
    {
        float2(-SampleSize, -SampleSize), float2(0.0f, -SampleSize), float2(SampleSize, -SampleSize),
		float2(-SampleSize, 0.0f), float2(0.0f, 0.0f), float2(SampleSize, 0.0f),
		float2(-SampleSize, +SampleSize), float2(0.0f, +SampleSize), float2(SampleSize, +SampleSize)
    };

    float R = 0.f;

    for (int i = 0; i < 9; i++)
    {
        R += SimpleShadowMap.SampleCmpLevelZero(
			ShadowSampler,
			ShadowPointHome.xy + ConstShadowSample[i],
			ShadowDepth).r;
    }

    // 添加max限制，以增加阴影细节
    // 最大値制限を追加して、シャドウのディテールを向上
    return saturate(R / 9.f);
}

float ProcessingOmnidirectionalSampleCubeMapShadow(float4 InWorldPosition, float3 InWorldLightPosition)
{
    float3 LightView = InWorldPosition.xyz - InWorldLightPosition;
	
    return SimpleShadowCubeMap.Sample(TextureSampler, LightView).r;
}

float ProcessingOmnidirectionalSampleCmpLevelZeroCubeMapShadow(float4 InWorldPosition, float3 InWorldLightPosition)
{
    float3 LightView = InWorldPosition.xyz - InWorldLightPosition;

    float ShadowDepth = 0.f;
    return SimpleShadowCubeMap.SampleCmpLevelZero(ShadowSampler, float4(LightView, 1.f), ShadowDepth).r;
}

//　笛卡尔转球面
//　デカルト座標から球面座標へ変換
float3 GetPointSphericalCoordinates(float3 InPoint)
{
    float AnglePre = (180.f / 3.1415926f);

    float R = sqrt(InPoint.x * InPoint.x + InPoint.y * InPoint.y + InPoint.z * InPoint.z);
    float Thata = acos(InPoint.y / R) * AnglePre;
    float Fai = atan2(InPoint.z, InPoint.x) * AnglePre;

    return float3(R, Thata, Fai);
}

bool IsAngleRange(float InAngle, float X, float Y)
{
    if (InAngle >= min(X, Y))
    {
        if (InAngle <= max(X, Y))
        {
            return true;
        }
    }

    return false;
}

bool IsAngleAxisRange(
    float InAngle,
    float X,
    float Y,
    float InCriticalValue,
    bool bComMin)
{
    if (bComMin) 
    {
        if (InAngle > InCriticalValue)
        {
            if (InAngle <= max(X, Y))
            {
                return true;
            }
        }
        else if (InAngle < InCriticalValue)
        {
            if (InAngle >= min(X, Y))
            {
                return true;
            }
        }
        else if (InAngle == 0.f)
        {
            return true;
        }
    }
    else
    {
        if (InAngle > 0.f) //+
        {
            if (InAngle >= max(X, Y))
            {
                if (InAngle <= InCriticalValue)
                {
                    return true;
                }
            }
        }
        else
        {
            if (InAngle <= min(X, Y))
            {
                if (InAngle >= -InCriticalValue)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool IsPointInCubeMapVieport(
    float InPointTheta,
    float InPointFai,
    float4 InCompareValue,
    int bAxisRange,
    float InCriticalValue,
    bool bComMin)
{
    if (IsAngleRange(InPointTheta, InCompareValue.x, InCompareValue.y))
    {
        if (bAxisRange == 0)
        {
            if (IsAngleAxisRange(InPointFai, InCompareValue.z, InCompareValue.w, InCriticalValue, bComMin))
            {
                return true;
            }
        }
        else if (bAxisRange == 1)
        {
            if (InPointFai >= InCompareValue.z)
            {
                if (InPointFai <= InCompareValue.w)
                {
                    return true;
                }
            }
        }
        else if (bAxisRange == 2)
        {
            if (InPointFai <= InCompareValue.z)
            {
                if (InPointFai >= InCompareValue.w)
                {
                    return true;
                }
            }
        }
        else
        {
            if (IsAngleRange(InPointFai, InCompareValue.z, InCompareValue.w))
            {
                return true;
            }
        }
    }

    return false;
}

struct FCubeMapAxialRangeR
{
    float4 PositiveX;
    float4 NegativeX;

    float4 PositiveY;
    float4 NegativeY;

    float4 PositiveZ;
    float4 NegativeZ;
};

int GetSampleCubeMapIndexR(float3 InPointPosition)
{
    FCubeMapAxialRangeR CubeMapAxialRangeRight;
    CubeMapAxialRangeRight.PositiveX = float4(45.f, 135.f, 45.f, -45.f);
    CubeMapAxialRangeRight.NegativeX = float4(45.f, 135.f, 135.f, -135.f); 
    CubeMapAxialRangeRight.PositiveY = float4(0.f, 45.f, 360.f, -360.f); 
    CubeMapAxialRangeRight.NegativeY = float4(135.f, 180.f, 360.f, -360.f); 
    CubeMapAxialRangeRight.PositiveZ = float4(45.f, 135.f, 45.f, 135.f);
    CubeMapAxialRangeRight.NegativeZ = float4(45.f, 135.f, -45.f, -135.f); 

    float3 Point = GetPointSphericalCoordinates(InPointPosition);

    float PointTheta = Point.y;
    float PointFai = Point.z;

    if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.PositiveX, 3, 0.f, true))
    {
        return 0;
    }
    else if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.NegativeX, 0, 180.f, false))
    {
        return 1;
    }
    else if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.PositiveY, 3, 0.f, false))
    {
        return 2;
    }
    else if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.NegativeY, 3, 0.f, false))
    {
        return 3;
    }
    else if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.PositiveZ, 1, 0.f, false))
    {
        return 4;
    }
    else if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.NegativeZ, 2, 0.f, false))
    {
        return 5;
    }

    return 6;
}

float4 DebugCubeVieport(int Index)
{
    if (Index == 0)
    {
        return float4(1.f, 0.f, 0.f, 1.f);
    }
    else if (Index == 1)
    {
        return float4(1.f, 1.f, 0.f, 1.f);
    }
    else if (Index == 2)
    {
        return float4(1.f, 0.f, 1.f, 1.f);
    }
    else if (Index == 3)
    {
        return float4(0.f, 1.f, 1.f, 1.f);
    }
    else if (Index == 4)
    {
        return float4(1.f, 0.5f, 1.f, 1.f);
    }
    else if (Index == 5)
    {
        return float4(0.5f, 1.f, 1.f, 1.f);
    }

    return float4(0.f, 0.f, 0.f, 1.f);
}

// 将uv坐标转为极坐标，用于计算旋转时的手柄面片角度
// UV座標を極座標に変換し、回転時のハンドルパッチ角度を計算するために使用
float SampleBuildAGTexture(float2 InTexCoord)
{
    float PI = 3.1415926f; //180°
    float PI2 = PI * 2.f; //360°

    float2 TexCoord = InTexCoord - 0.5f;
    float2 ATan2Value = atan2(TexCoord.x, TexCoord.y); //
    float A = ATan2Value.y;

    float R = 0.f;
    if (A > 0.f)
    {
        R = ATan2Value / PI2; //把正值映射到 0-1　　//正の値を0-1にマッピング
    }
    else if (A < 0.f)
    {
        R = (ATan2Value.y + PI2) / PI2; //映射到 0-1空间里面　　//0-1空間にマッピング
    }

    return R;
}

#endif
