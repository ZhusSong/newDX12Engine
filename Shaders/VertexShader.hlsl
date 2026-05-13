#include "Material.hlsli"
#include "PBR.hlsl"
#include "Fog.hlsli"
#include "ShadowFunction.hlsli"


struct MeshVertexIn
{
	float3 Position : POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float3 UTangent: TANGENT;
	float2 TexCoord: TEXCOORD;
};

struct MeshVertexOut
{
	float4 WorldPosition : POSITION;
	float4 TexPositionHome : POSITION1;
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float3 UTangent : TANGENT;
	float2 TexCoord: TEXCOORD;
};

MeshVertexOut VertexShaderMain(MeshVertexIn MV)
{
    MaterialConstBuffer MatConstBuffer = Materials[MaterialIndex];
    
	
    MeshVertexOut MOut;
    
	// 色
    MOut.Color = MV.Color;
    
	// 世界坐标
    // ワールド座標
    MOut.WorldPosition = mul(float4(MV.Position, 1.f), WorldMatrix);

   	MOut.TexPositionHome = mul(MOut.WorldPosition, TexViewProjectionMatrix);

	// 变换到齐次剪辑空间
    // 同次クリップ空間に変換
    MOut.Position = mul(MOut.WorldPosition, ViewProjectionMatrix);

    if (MatConstBuffer.MaterialType == 13)
    {
        MOut.Normal = MV.Normal;
    }
    else
    {
		// 转法线
        // 法線を変換
       	MOut.Normal = mul(MV.Normal, (float3x3)NormalTransformation);
    }
	
	// 切线
    // 接線
    MOut.UTangent = mul(MV.UTangent, (float3x3)NormalTransformation);

	// ui座標
    float4 MyTexCoord = mul(float4(MV.TexCoord, 0.0f, 1.f), ObjectTextureTransform);
    MOut.TexCoord = mul(MyTexCoord, MatConstBuffer.TransformInformation).xy;

    return MOut;
}

float4 PixelShaderMain(MeshVertexOut MVOut) : SV_TARGET
{
    MaterialConstBuffer MatConstBuffer = Materials[MaterialIndex];

    MVOut.TexPositionHome /= MVOut.TexPositionHome.w;
    float AmbientAccessibility = SimpleSSAOMap.Sample(TextureSampler, MVOut.TexPositionHome.xy, 0.0f).r;
	
    // 返回阴影贴图采样
    // シャドウマップサンプリングを返す
    if (MatConstBuffer.MaterialType == 101)
    {
      return float4(AmbientAccessibility, AmbientAccessibility, AmbientAccessibility, 1.f);
    }
    FMaterial Material;

	//BaseColor
    Material.BaseColor = GetMaterialBaseColor(MatConstBuffer, MVOut.TexCoord);


	//BaseColor
    if (MatConstBuffer.MaterialType == 12)
    {
        // 反射
        float4 Specular = GetMaterialSpecular(MatConstBuffer, MVOut.TexCoord);
        return Material.BaseColor * Specular + Material.BaseColor + 0.1f;
    }
    else if (MatConstBuffer.MaterialType == 13)
    {
        return float4(MVOut.Normal, 1.f);
    }
    else if (MatConstBuffer.MaterialType == 14)
    {
        return float4(MVOut.Normal, 1.f);
    }

    float4 AmbientLight = { 0.15f, 0.15f, 0.15f, 1.0f };
    float3 ModelNormal = normalize(MVOut.Normal);

	//　获取法线 如果设置了法线贴图就获取法线贴图
    //　法線を取得 法線マップが設定されている場合は法線マップを取得
    ModelNormal = GetMaterialNormals(MatConstBuffer, MVOut.TexCoord, ModelNormal, MVOut.UTangent);

    float DotValue = 0;

    float4 FinalColor = { 0.f, 0.f, 0.f, 1.f };
    

    for (int i = 0; i < 16; i++)
    {
        if (length(SceneLights[i].LightIntensity.xyz) > 0.f)
        {
            float4 Specular = GetMaterialSpecular(MatConstBuffer, MVOut.TexCoord);

            float3 NormalizeLightDirection = normalize(GetLightDirection(SceneLights[i], MVOut.WorldPosition.xyz));

            float4 LightStrength = ComputeLightStrength(SceneLights[i], ModelNormal, MVOut.WorldPosition.xyz, NormalizeLightDirection);

			//　兰伯特
            //　ランバート
            if (MatConstBuffer.MaterialType == 0)
            {
                DotValue = pow(max(dot(ModelNormal, NormalizeLightDirection), 0.0), 2.f);
            }
			//　半兰伯特
            //　ハーフランバート
            else if (MatConstBuffer.MaterialType == 1)
            {
                float DiffuseReflection = dot(ModelNormal, NormalizeLightDirection);
                DotValue = max((DiffuseReflection * 0.5f + 0.5f), 0.0); //[-1,1] => [0,1]
            }
			//phong
            else if (MatConstBuffer.MaterialType == 2)
            {
                float3 ReflectDirection = normalize(-reflect(NormalizeLightDirection, ModelNormal));
                float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);

                DotValue = pow(max(dot(ModelNormal, NormalizeLightDirection), 0.0), 2.f);

                float MaterialShininess = 1.f - saturate(MatConstBuffer.MaterialRoughness);
                float M = MaterialShininess * 100.f;

                Specular *= saturate((M + 2.0f) * pow(max(dot(ViewDirection, ReflectDirection), 0.f), M) / 3.1415926);
            }
			//blinn-phong 
            else if (MatConstBuffer.MaterialType == 3)
            {
                float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
                float3 HalfDirection = normalize(ViewDirection + NormalizeLightDirection);

				// 先半兰博特化 再减去0.2f曝光的，再平方，变得更柔和
                // まずハーフランバート化し、次に0.2fの露出を減算し、さらに二乗して、より柔らかくする
                DotValue = pow(max(0.0, (dot(ModelNormal, NormalizeLightDirection) * 0.5f + 0.5f) - 0.2f), 2);

                float MaterialShininess = 1.f - saturate(MatConstBuffer.MaterialRoughness);
                float M = MaterialShininess * 100.f;

				// c=(m+2.f/PI) 归一化系数 后面会详细讲解推导
                // c=(m+2.f/PI) 正規化係数 後で詳細な導出を説明
                Specular *= saturate((M + 2.0f) * pow(max(dot(HalfDirection, ModelNormal), 0.f), M) / 3.1415926);
            }
            //Wrap
            else if (MatConstBuffer.MaterialType == 4)
            {
				//	float WrapValue = 1.f;

                float WrapValue = 2.6f;

                float DiffuseReflection = dot(ModelNormal, NormalizeLightDirection);
                DotValue = max((DiffuseReflection + WrapValue) / (1.f + WrapValue), 0.0); //[-1,1] => [0,1]
            }
            else if (MatConstBuffer.MaterialType == 5)//Minnaert 
            {
                float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);

                float DotLight = pow(max(dot(ModelNormal, NormalizeLightDirection), 0.0), 2.f);
                float DotView = max(dot(ModelNormal, ViewDirection), 0.0);

                float MaterialShininess = 1.f - saturate(MatConstBuffer.MaterialRoughness);
                float M = MaterialShininess * 10.f;
                DotValue = saturate(DotLight * pow(DotLight * DotView, M));
            }
            else if (MatConstBuffer.MaterialType == 6)//Banded 
            {
                if (i == 0)
                {
					// 融入半兰伯特
                    // ハーフランバートを組み込む
                    float DiffuseReflection = (dot(ModelNormal, NormalizeLightDirection) + 1.f) * 0.5f;

                    float Layered = 4.f;

                    DotValue = floor(DiffuseReflection * Layered) / Layered;
                }
            }
            else if (MatConstBuffer.MaterialType == 7)//Banded 
            {
                if (i == 0)
                {
                    float4 Color2 = { 245.f / 255.f, 88.f / 255.f, .0f, 1.f };

					// 灯光点乘值
                    // ライトの内積値
                    float LightDotValue = dot(ModelNormal, NormalizeLightDirection);

                    float DiffuseReflection = (LightDotValue + 1.f) * 0.5f;

                    float Layered = 7.f;
                    DotValue = floor(DiffuseReflection * Layered) / Layered;

                    Material.BaseColor = lerp(Color2, Material.BaseColor, LightDotValue);
                }
            }
            //最終Banded 
            else if (MatConstBuffer.MaterialType == 8)//鏈€绲侭anded 
            {
                if (i == 0)
                {
					//　融入半兰伯特
                    //　ハーフランバートを組み込む
                    float DiffuseReflection = (dot(ModelNormal, NormalizeLightDirection) + 1.f) * 0.5f;
                    
                    float Layered = 4.f;
                    DotValue = floor(DiffuseReflection * Layered) / Layered;

					//　菲尼尔效果
                    //　フレネル効果
                    float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
                    float3 F0 = { 0.05f, 0.05f, 0.05f };
                    Specular.xyz = FresnelSchlickMethod(F0, ModelNormal, ViewDirection, 3).xyz;

					//　反射
                    float3 ReflectDirection = normalize(-reflect(NormalizeLightDirection, ModelNormal));

					//　highlight
                    if (DotValue > 0.f)
                    {
                        float MaterialShininess = 1.f - saturate(MatConstBuffer.MaterialRoughness);
                        float M = MaterialShininess * 60.f;

                        Specular += saturate(pow(max(dot(ViewDirection, ReflectDirection), 0.f), M) / 0.0314f);
                    }
                }
            }
            else if (MatConstBuffer.MaterialType == 9)// Back Light
            {
				//phong
                float3 ReflectDirection = normalize(-reflect(NormalizeLightDirection, ModelNormal));
                float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);

				//wrap
                float WrapValue = 1.2f;
                float DiffuseReflection = dot(ModelNormal, NormalizeLightDirection);
                DotValue = max((DiffuseReflection + WrapValue) / (1.f + WrapValue), 0.0); //[-1,1] => [0,1]

				//highlight
                if (DotValue > 0.f)
                {
                    float MaterialShininess = 1.f - saturate(MatConstBuffer.MaterialRoughness);
                    float M = MaterialShininess * 100.f;

                    Specular = saturate(pow(max(dot(ViewDirection, ReflectDirection), 0.f), M));
                }

				// 模拟透射效果
                // 透過効果をシミュレート
                float SSSValue = 1.3f;
                float TransmissionIntensity = 2.f;
                float TransmissionScope = 1.5f;

                float3 LightNormalizeValue = -normalize(ModelNormal * SSSValue + NormalizeLightDirection);
                DotValue = DotValue + pow(saturate(dot(LightNormalizeValue, ViewDirection)), TransmissionScope) * TransmissionIntensity;
            }
            else if (MatConstBuffer.MaterialType == 10)// kajiya-kay
            {
                float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
                float3 HalfDirection = normalize(ViewDirection + NormalizeLightDirection);

                float3 StrandTangent = GetOrthonormalizedTangent(MVOut.UTangent, ModelNormal);

                // Param0: tangent shift, Param1: primary lobe strength, Param2: secondary lobe strength.
                float TangentShift = abs(MatConstBuffer.Param0) > 0.0001f ? MatConstBuffer.Param0 : -0.2f;
                float PrimarySpecularStrength = MatConstBuffer.Param1 > 0.0001f ? MatConstBuffer.Param1 : 1.0f;
                float SecondarySpecularStrength = MatConstBuffer.Param2 > 0.0001f ? MatConstBuffer.Param2 : 0.35f;

                float MaterialShininess = 1.f - saturate(MatConstBuffer.MaterialRoughness);
                float PrimaryExponent = lerp(18.0f, 128.0f, MaterialShininess);
                float SecondaryExponent = max(PrimaryExponent * 0.35f, 6.0f);

                float3 PrimaryTangent = ShiftAnisotropyDirection(StrandTangent, ModelNormal, TangentShift);
                float3 SecondaryTangent = ShiftAnisotropyDirection(StrandTangent, ModelNormal, -TangentShift * 0.5f);

                float FiberDiffuse = GetKajiyaKayDiffuse(StrandTangent, NormalizeLightDirection);
                float LightFacing = saturate(dot(ModelNormal, NormalizeLightDirection));
                DotValue = FiberDiffuse * LightFacing;

                float PrimarySpecular = GetKajiyaKaySpecular(PrimaryTangent, HalfDirection, PrimaryExponent);
                float SecondarySpecular = GetKajiyaKaySpecular(SecondaryTangent, HalfDirection, SecondaryExponent);
                float ViewFacing = saturate(dot(ModelNormal, ViewDirection));
                float HalfAlignment = saturate(dot(ViewDirection, NormalizeLightDirection) * 0.5f + 0.5f);

                float AnisotropicSpecular =
                    (PrimarySpecularStrength * PrimarySpecular + SecondarySpecularStrength * SecondarySpecular) *
                    LightFacing *
                    ViewFacing *
                    HalfAlignment;

                Specular *= AnisotropicSpecular;
            }
            else if (MatConstBuffer.MaterialType == 11)//OrenNayar
            {
                float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);

                float NormalLight = saturate(pow(max(dot(ModelNormal, NormalizeLightDirection), 0.0), 2.f)); //兰伯特
                float NormalView = saturate(dot(ModelNormal, ViewDirection));

                float Phiri =
					length(ViewDirection - ModelNormal * NormalView) +
					length(NormalizeLightDirection - ModelNormal * NormalLight);

                float ACosNormalView = acos(NormalView); //[0,1]
                float ACosNormalLight = acos(NormalLight);

                float Alpha = max(ACosNormalView, ACosNormalLight);
                float Beta = min(ACosNormalView, ACosNormalLight);

                float MyRoughness = pow(MatConstBuffer.MaterialRoughness, 2);

                float A = 1 - 0.5f * (MyRoughness / (MyRoughness + 0.33f));
                float B = 0.45f * (MyRoughness / (MyRoughness + 0.09f));

                DotValue = NormalLight * (A + B * max(0, Phiri) * sin(Alpha) * tan(Beta));
            }
            else if (MatConstBuffer.MaterialType == 15)//透明
            {
                //float DiffuseReflection = dot(ModelNormal, NormalizeLightDirection);
                //DotValue = max((DiffuseReflection * 0.5f + 0.5f), 0.0); //[-1,1] => [0,1]
            }
            else if (MatConstBuffer.MaterialType == 20)//PBR
            {
                float3 L = NormalizeLightDirection;
                float3 V = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
                float3 H = normalize(V + L);
                float3 N = ModelNormal;

                float PI = 3.1415926;

                float Roughness = MatConstBuffer.MaterialRoughness;
                float3 Metallic = MatConstBuffer.Metallicity;

                float4 D = GetDistributionGGX(N, H, Roughness);

                float3 F0 = 0.04f;
                F0 = lerp(F0, MatConstBuffer.BaseColor.rgb, Metallic);
                float4 F = float4(FresnelSchlickMethod(F0, N, V, 5), 1.0f);

                float4 G = GSmith(N, V, L, Roughness);

                float LoH = saturate(dot(L, H));
                float NoV = saturate(dot(N, V));
                float NoL = saturate(dot(N, L));

                float3 FIndirect = GetIndirectLight(LoH, F0, Roughness);
                float3 IndirKS = GetDirectLight(NoV, F0, Roughness);

                float4 Kd = 1 - float4(FIndirect, 1.f);
                Kd *= (1 - float4(IndirKS, 1.f)) * (1 - float4(Metallic, 1.f));

                float3 Diffuse = Kd.rgb * GetDiffuseLambert(MatConstBuffer.BaseColor.rgb);

                float4 Value = (D * F * G) / (4 * (NoV * NoL));
                Specular = float4(Value.rgb, 1.f);

                float3 Radiance = LightStrength.xyz;
                float3 MyColor = (Diffuse + Specular.xyz) * NoL * Radiance;

				//IBL
                return float4(MyColor.xyz, 1.0f);
            }
			// 菲尼尔
            // フレネル
            else if (MatConstBuffer.MaterialType == 100)
            {
                float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
                DotValue = pow(1.f - max(dot(ModelNormal, ViewDirection), 0.0), 2.f);
				
				// Schlick 菲尼尔方法
                // Schlick フレネル
				//float3 F0 = { 0.1f,0.1f,0.1f };
				//Specular.xyz = FresnelSchlickMethod(F0, ModelNormal, ViewDirection, 3).xyz;
            }

            // 漫反射
            // 拡散反射
            float4 Diffuse = Material.BaseColor;
            // highlight
			Specular = saturate(Specular);

            // shadow
            float ShadowFactor = 1.f;
			if (SceneLights[i].LightType == 1)
			{
                // 点光源cubemap阴影
                // 点光源のcubemapシャドウ
				ShadowFactor = ProcessingOmnidirectionalSampleCmpLevelZeroCubeMapShadow(MVOut.WorldPosition, SceneLights[i].Position);
			}
			else
			{
                // 此处可用ShadowFunction中的多种方式计算阴影
                // ここではShadowFunction内の複数の方法でシャドウを計算可能
				//float ShadowFactor = GetShadowFactor(MVOut.WorldPosition, SceneLights[i].ShadowTransform);
				//float ShadowFactor = GetShadowFactor_PCF_Sample4(MVOut.WorldPosition, SceneLights[i].ShadowTransform);
				ShadowFactor = GetShadowFactor_PCF_Sample9(MVOut.WorldPosition, SceneLights[i].ShadowTransform);
			}
	
            FinalColor += ShadowFactor * (saturate((Diffuse + Specular) * LightStrength * DotValue));
        }
    }
    float AOVisibility = saturate(AmbientAccessibility);
    float GroundReceiver = pow(saturate(ModelNormal.y), 3.0f);
    float AOAmbientFactor = lerp(1.0f, lerp(0.08f, 1.0f, AOVisibility), GroundReceiver);
    float AOSurfaceFactor = lerp(1.0f, lerp(0.28f, 1.0f, pow(AOVisibility, 1.08f)), GroundReceiver);
    float4 Ambient = AOAmbientFactor * AmbientLight * Material.BaseColor;

    // 最终颜色
    // 最終色
    MVOut.Color = (FinalColor + Ambient) * AOSurfaceFactor; 
	
    
    switch (MatConstBuffer.MaterialType)
    {
        case 2:
        case 3:
        case 9:
            {      
			    //计算反射
                //反射を計算 
                float3 ReflectionColor = GetReflectionColor(MatConstBuffer, ModelNormal, MVOut.WorldPosition.xyz);
                MVOut.Color.xyz += ReflectionColor;
                break;
            }
        // 透明算法
        // 透明アルゴリズム
        case 15:
            {
			    // 计算折射
                // 屈折を計算
                float3 NewRefract = GetRefract(ModelNormal, MVOut.WorldPosition.xyz, MatConstBuffer.Refraction);
                float3 SampleRefractColor = GetReflectionSampleColor(ModelNormal, NewRefract);

			    // 反射を計算
                float3 NewReflect = GetReflect(ModelNormal, MVOut.WorldPosition.xyz);
                float3 SampleReflectionColor = GetReflectionSampleColor(ModelNormal, NewReflect);
			
			    // 计算A通道
                // Aチャンネルを計算
                float3 V = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
                float Shininess = GetShininess(MatConstBuffer);
                float3 FresnelFactor = FresnelSchlickFactor(MatConstBuffer, ModelNormal, V);

                float3 Color = lerp(SampleRefractColor, SampleReflectionColor, pow(Shininess * FresnelFactor, 2));

                MVOut.Color.xyz += Color;
                break;
            }
    }

    if (MatConstBuffer.MaterialType == 15)
    {
		// 透明
        MVOut.Color.a = MatConstBuffer.Transparency;
    }
    else
    {
		// 非透明
        MVOut.Color.a = Material.BaseColor.a;
    }
	
	
	// 计算フォグ
    MVOut.Color = GetFogValue(MVOut.Color, (float3)MVOut.WorldPosition);
    
    return MVOut.Color;
}
