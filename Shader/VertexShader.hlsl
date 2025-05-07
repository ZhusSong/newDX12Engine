#include "Material.hlsl"
#include "Light.hlsl"

cbuffer ObjectConstBuffer : register(b0) //b0->b14
{
    float4x4 WorldMatrix;
}

cbuffer ViewportConstBuffer : register(b1) 
{
    float4x4 ViewProjectionMatrix;
}

cbuffer MaterialConstBuffer : register(b2)
{
    int MaterialType;
    
    float4 BaseColor;
    float4x4 TransformInformation;
}

cbuffer LightConstBuffer : register(b3) 
{
    float3 LightIntensity;
    float3 LightDirection;
}
struct MeshVertexIn
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
};

struct MeshVertexOut
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
};

MeshVertexOut VertexShaderMain(MeshVertexIn MV)
{
    MeshVertexOut Out;

    float4 Position = mul(float4(MV.Position, 1.f), WorldMatrix);
	//Out.Position = Position;
    Out.Position = mul(Position, ViewProjectionMatrix);

	//转法线
    Out.Normal = mul(MV.Normal, (float3x3) WorldMatrix);

   
    Out.Color = MV.Color;

    return Out;
}

float4 PixelShaderMain(MeshVertexOut MVOut) : SV_TARGET
{
    float4 AmbientLight = { 0.15f, 0.15f, 0.25f, 1.0f };

    float3 ModelNormal = normalize(MVOut.Normal);
    float3 NormalizeLightDirection = normalize(-LightDirection);
    
    FMaterial Material;
    Material.BaseColor = BaseColor;
    
    float DotValue = 0;
    DotValue = max(dot(ModelNormal, NormalizeLightDirection), 0.0);
    if (MaterialType == 0)//兰伯特
    {
        DotValue = max(dot(ModelNormal, NormalizeLightDirection), 0.0);
    }
    else if (MaterialType == 1)//半兰伯特
    {
        float DiffuseReflection = dot(ModelNormal, NormalizeLightDirection);
        DotValue = max((DiffuseReflection * 0.5f + 0.5f), 0.0); //[-1,1] => [0,1]
    }
	
	// 最终颜色
    MVOut.Color = Material.BaseColor * DotValue + AmbientLight * Material.BaseColor;
	
    
    return MVOut.Color;
}