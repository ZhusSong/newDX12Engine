#ifndef ShaderCommon
#define ShaderCommon


#include "Light.hlsli"



#ifndef TEXTURE2D_MAP_NUM
#define TEXTURE2D_MAP_NUM 20  
#endif

#ifndef CUBE_MAP_NUM
#define CUBE_MAP_NUM 20  
#endif

//#ifndef START_UP_FOG
//#define START_UP_FOG 1  
//#endif

SamplerState TextureSampler : register(s0);

SamplerState AnisotropicSampler : register(s1);

SamplerComparisonState ShadowSampler : register(s2);

TextureCube SimpleCubeMap : register(t0); //根签名位置6

TextureCube SimpleShadowCubeMap : register(t1); //点光源 ShadowCubemap 根签名位置8
Texture2D SimpleShadowMap : register(t2); // 聚光灯和平行光shadowmap 根签名位置7

Texture2D SimpleTexture2DMap[TEXTURE2D_MAP_NUM] : register(t3);



cbuffer ObjectConstBuffer : register(b0) 
{
    float4x4 WorldMatrix;          //世界矩阵 处理法线不等比缩放 
    float4x4 ObjectTextureTransform;
    float4x4 NormalTransformation;  //法线矩阵 处理法线不等比缩放
    uint MaterialIndex;
    uint RR1;
    uint RR2;
    uint RR3;
};

cbuffer ViewportConstBuffer : register(b1) 
{
    float4 ViewportPosition;
    float4x4 ViewProjectionMatrix;
};

cbuffer LightConstBuffer : register(b2) 
{
    Light SceneLights[16];
};

cbuffer FogConstBuffer : register(b3)
{
	float3 FogColor;
	float FogStart;

	float FogRange;
	float FogHeight;
	float FogTransparentCoefficient;
	float xxx2;
};

struct MaterialConstBuffer
{
    int MaterialType;
    float MaterialRoughness;
    int BaseColorIndex;
    int NormalIndex;

    int SpecularIndex;
    float Param0; //自定义
    float Param1; //自定义
    float Param2; //自定义

    float4 BaseColor;

    float3 SpecularColor;
    float Refraction;

    float3 FresnelF0;
    float Transparency;
    float4x4 TransformInformation;

    float3 Metallicity;
    float XXX5;
    
};

StructuredBuffer<MaterialConstBuffer> Materials : register(t0, Space1);
#endif