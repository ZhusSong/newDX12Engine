#include "Material.hlsl"
#include "Light.hlsl"

cbuffer ObjectConstBuffer : register(b0) //b0->b14
{
    float4x4 WorldMatrix;
}

cbuffer ViewportConstBuffer : register(b1)
{
    float4 ViewportPosition;
    float4x4 ViewProjectionMatrix;
}

cbuffer MaterialConstBuffer : register(b2)
{
    int MaterialType;
    
    float4 BaseColor;
    float MaterialRoughness;
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
    float4 WorldPosition : POSITION;
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
};

MeshVertexOut VertexShaderMain(MeshVertexIn MV)
{
    MeshVertexOut Out;

   //世界坐标
    Out.WorldPosition = mul(float4(MV.Position, 1.f), WorldMatrix);

	//变换到齐次剪辑空间
    Out.Position = mul(Out.WorldPosition, ViewProjectionMatrix);

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
    
    float4 Specular = { 0.f, 0.f, 0.f, 1.f };
    if (MaterialType == 0)//兰伯特
    {
        DotValue = max(dot(ModelNormal, NormalizeLightDirection), 0.0);
    }
    else if (MaterialType == 1)//半兰伯特
    {
        float DiffuseReflection = dot(ModelNormal, NormalizeLightDirection);
        DotValue = max((DiffuseReflection * 0.5f + 0.5f), 0.0); //[-1,1] => [0,1]
    }
    else if (MaterialType == 2)//phong
    {
        float3 ReflectDirection = normalize(-reflect(NormalizeLightDirection, ModelNormal));
        float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);

        DotValue = max(dot(ModelNormal, NormalizeLightDirection), 0.0);

        if (DotValue > 0.f)
        {
            float MaterialShininess = 1.f - saturate(MaterialRoughness);
            float M = MaterialShininess * 100.f;

            Specular = pow(max(dot(ViewDirection, ReflectDirection), 0.f), M);
        }
    }
	
	// 最终颜色
    MVOut.Color = Material.BaseColor * DotValue + //漫反射
		AmbientLight * Material.BaseColor + //间接光
		Specular * Material.BaseColor; //高光
	
    
    return MVOut.Color;
}