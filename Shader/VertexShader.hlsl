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
    float3 UTangent : TANGENT;
};

struct MeshVertexOut
{
    float4 WorldPosition : POSITION;
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float3 UTangent : TANGENT;
};

MeshVertexOut VertexShaderMain(MeshVertexIn MV)
{
    MeshVertexOut Out;

   //世界坐标
    Out.WorldPosition = mul(float4(MV.Position, 1.f), WorldMatrix);

	//变换到齐次剪辑空间
    Out.Position = mul(Out.WorldPosition, ViewProjectionMatrix);
    Out.Color.rgb = MV.Normal.rgb;
    
    // 是否以自身法线显示
    if (MaterialType == 13)
    {
        Out.Normal = MV.Normal;
    }
    else
    {
		// 转法线
        Out.Normal = mul(MV.Normal, (float3x3) WorldMatrix);
    }
    // 切线
    Out.UTangent = MV.UTangent;

    return Out;
}

float4 PixelShaderMain(MeshVertexOut MVOut) : SV_TARGET
{
    FMaterial Material;
    Material.BaseColor = BaseColor;
    
    // 不收光照影响的BaseColor
    if (MaterialType == 12)
    {
        return Material.BaseColor;
    }
    else if (MaterialType == 13)
    {
        return float4(MVOut.Normal, 1.f);
    }
    else if (MaterialType == 14)
    {
        return float4(MVOut.Normal, 1.f);
    }
    
    float4 AmbientLight = { 0.15f, 0.15f, 0.25f, 1.0f };

    float3 ModelNormal = normalize(MVOut.Normal);
    float3 NormalizeLightDirection = normalize(-LightDirection);
    
    
    float DotValue = 0;
    float4 Specular = { 0.f, 0.f, 0.f, 1.f };
    
    if (MaterialType == 0)// Lambert 兰伯特
    {
        DotValue = max(dot(ModelNormal, NormalizeLightDirection), 0.0);
    }
    else if (MaterialType == 1)// Half Lambert半兰伯特
    {
        float DiffuseReflection = dot(ModelNormal, NormalizeLightDirection);
        DotValue = max((DiffuseReflection * 0.5f + 0.5f), 0.0); //[-1,1] => [0,1]
    }
    else if (MaterialType == 2)// Phong
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
    else if (MaterialType == 3)// binn phong
    {
        float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
        float3 HalfDirection = normalize(ViewDirection + NormalizeLightDirection);

        DotValue = max(dot(ModelNormal, NormalizeLightDirection), 0.0);

        // 高光
        if (DotValue > 0.f)
        {
            float MaterialShininess = 1.f - saturate(MaterialRoughness);
            float M = MaterialShininess * 100.f;

            Specular = pow(max(dot(HalfDirection, ModelNormal), 0.f), M);
        }
    }
    else if (MaterialType == 4)// wrap 早期模拟皮肤材质
    {
        float WrapValue = 1.5f; // 1.0f=half lambert 
        float DiffuseReflection = dot(ModelNormal, NormalizeLightDirection);
        DotValue = max((DiffuseReflection + WrapValue) / (1.0f + WrapValue), 0.0); //[-1,1] => [0,1]
    }
    else if (MaterialType == 5)// Minnaert 早期模拟月球材质
    {
        float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
        
        float DotLight = max(dot(ModelNormal, NormalizeLightDirection), 0.0f);
        float DotView = max(dot(ModelNormal, ViewDirection), 0.0f);
        
        float MeterialShiness = 1.0f - saturate(MaterialRoughness);
        float M = MeterialShiness * 1.0f;
        DotValue = saturate(DotLight * pow(DotLight * DotView, M)); //[-1,1] => [0,1]
   
    }
    else if (MaterialType == 6)// Banded 卡通材质
    {
        // add half lambert
        float DiffuseReflection = (dot(ModelNormal, NormalizeLightDirection) + 1.0f) * 0.5f;
        
        
        float Layered = 4.0f;
        
        DotValue = floor(DiffuseReflection * Layered) / Layered;

    }
    else if (MaterialType == 7)// GradualBanded 卡通材质
    {
        float4 Color2 = { 245.f / 255.f, 88.f / 255.f, .0f, 1.f };
        // add half lambert
        
        float LightDotValue = dot(ModelNormal, NormalizeLightDirection);
        float DiffuseReflection = (LightDotValue + 1.0f) * 0.5f;
        
       
        float Layered = 7.0f;
        DotValue = floor(DiffuseReflection * Layered) / Layered;

        Material.BaseColor = lerp(Color2, Material.BaseColor, LightDotValue);

    }
    else if (MaterialType == 8)// FinalBanded 卡通材质
    {
        // add half lambert
        float DiffuseReflection = (dot(ModelNormal, NormalizeLightDirection) + 1.f) * 0.5f;
        
		// 分层思想
        float Layered = 4.f;

        DotValue = floor(DiffuseReflection * Layered) / Layered;
        
		// 菲尼尔效果
        float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
        float3 F0 = { 0.05f, 0.05f, 0.05f };
        Specular.xyz = FresnelSchlickMethod(F0, ModelNormal, ViewDirection, 3).xyz;
        
        // 反射
        float3 ReflectDirection = normalize(-reflect(NormalizeLightDirection, ModelNormal));
            
        // 高光
        if (DotValue > 0.f)
        {
            float MaterialShininess = 1.f - saturate(MaterialRoughness);
            float M = MaterialShininess * 80.f;

            Specular = Specular + pow(max(dot(ViewDirection, ReflectDirection), 0.f), M) / 0.032f;
        }

    }
    else if (MaterialType == 9)//   back  玉石透射材质
    {
        //phong
        float3 ReflectDirection = normalize(-reflect(NormalizeLightDirection, ModelNormal));
        float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
        
       //wrap
        float WrapValue = 1.2f;
        float DiffuseReflection = dot(ModelNormal, NormalizeLightDirection);
        DotValue = max((DiffuseReflection + WrapValue) / (1.f + WrapValue), 0.0); //[-1,1] => [0,1]

        
       //高光
        if (DotValue > 0.f)
        {
            float MaterialShininess = 1.f - saturate(MaterialRoughness);
            float M = MaterialShininess * 100.f;

            Specular = pow(max(dot(ViewDirection, ReflectDirection), 0.f), M);
        }

		//模拟透射效果
        float SSSValue = 1.3f;
        float TransmissionIntensity = 2.f;
        float TransmissionScope = 1.5f;

        float3 LightNormalizeValue = -normalize(ModelNormal * SSSValue + NormalizeLightDirection);
        DotValue = DotValue + pow(saturate(dot(LightNormalizeValue, ViewDirection)), TransmissionScope) * TransmissionIntensity;
    }
    else if (MaterialType == 10) //各向异性 Kajiya-Kay Shading Model（卡吉雅模型）
    {
      // 后续添加
    }
    
    //GDC 模拟粗糙表面
    else if (MaterialType == 11)
    {
        float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
		
        float NormalLight = saturate(dot(ModelNormal, NormalizeLightDirection)); //兰伯特
        float NormalView = saturate(dot(ModelNormal, ViewDirection));

        float Phiri =
			length(ViewDirection - ModelNormal * NormalView) *
			length(NormalizeLightDirection - ModelNormal * NormalLight);
		
        float ACosNormalView = acos(NormalView); //[0,1]
        float ACosNormalLight = acos(NormalLight);

        float Alpha = max(ACosNormalView, ACosNormalLight);
        float Beta = min(ACosNormalView, ACosNormalLight);

        float MyRoughness = pow(MaterialRoughness, 2);

        float A = 1 - 0.5f * (MyRoughness / (MyRoughness + 0.33f));
        float B = 0.45f * (MyRoughness / (MyRoughness + 0.09f));

        DotValue = NormalLight * (A + B * max(0, Phiri) * sin(Alpha) * tan(Beta));
    }
    //else if (MaterialType == 20)// PBR
    //{
    //    float3 L = NormalizeLightDirection;
    //    float3 V = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
    //    float3 H = normalize(V + L);
    //    float3 N = ModelNormal;
      
    //    float PI = 3.1415926;
        
    //    float Roughness = 0.9F;
    //    float Metallic = 0.3f;
    //}
    else if (MaterialType == 100)// Fresnel 菲尼尔
    {
        float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);

        float3 F0 = { 0.1f, 0.1f, 0.1f };
        
        Specular.xyz = FresnelSchlickMethod(F0, ModelNormal, ViewDirection, 5).xyz;

    }
    
    
    
    
	// 最终颜色
    MVOut.Color = Material.BaseColor * DotValue + //漫反射
		AmbientLight * Material.BaseColor + //间接光
		Specular * Material.BaseColor; //高光
	
    
    return MVOut.Color;
}