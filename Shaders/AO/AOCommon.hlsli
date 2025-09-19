#ifndef AOCOMMON_HLSL
#define AOCOMMON_HLSL

#ifndef SAMPLE_VOLUME_NUM
#define SAMPLE_VOLUME_NUM 14
#endif

SamplerState TextureSampler : register(s0);
SamplerState AnisotropicSampler : register(s1);
SamplerComparisonState ShadowSampler : register(s2);
SamplerState DepthSampler : register(s3);

Texture2D SampleNormalMap : register(t0);
Texture2D SampleDepthMap : register(t1);
Texture2D SampleNoiseMap : register(t2);

cbuffer CBufferSSAOView : register(b0)
{
    float4x4 ProjectionMatrix;
    float4x4 InversiveProjectionMatrix;
    float4x4 TexProjectionMatrix;

    float OcclusionRadius;
    float OcclusionStart;
    float OcclusionEnd;
    float ObscurationThreshold;
    
    // 采样数据
    float4 SampleVolumeBuffer[SAMPLE_VOLUME_NUM];
}

static const float2 TextureCoordinates[6] =
{
    float2(0.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 0.0f),
	float2(1.0f, 1.0f)
};

#endif