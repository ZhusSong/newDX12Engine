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

	// 鑹?
    MOut.Color = MV.Color;

	// 涓栫晫鍧愭爣
    // 銉兗銉儔搴ф
    MOut.WorldPosition = mul(float4(MV.Position, 1.f), WorldMatrix);

   	MOut.TexPositionHome = mul(MOut.WorldPosition, TexViewProjectionMatrix);

	// 鍙樻崲鍒伴綈娆″壀杈戠┖闂?
    // 鍚屾銈儶銉冦儣绌洪枔銇鎻?
    MOut.Position = mul(MOut.WorldPosition, ViewProjectionMatrix);

    if (MatConstBuffer.MaterialType == 13)
    {
        MOut.Normal = MV.Normal;
    }
    else
    {
		// 杞硶绾?
        // 娉曠窔銈掑鎻?
       	MOut.Normal = mul(MV.Normal, (float3x3)NormalTransformation);
    }

	// 鍒囩嚎
    // 鎺ョ窔
    MOut.UTangent = mul(MV.UTangent, (float3x3)NormalTransformation);

	// ui搴ф
    float4 MyTexCoord = mul(float4(MV.TexCoord, 0.0f, 1.f), ObjectTextureTransform);
    MOut.TexCoord = mul(MyTexCoord, MatConstBuffer.TransformInformation).xy;

    return MOut;
}

float4 PixelShaderMain(MeshVertexOut MVOut) : SV_TARGET
{
    MaterialConstBuffer MatConstBuffer = Materials[MaterialIndex];

    if (PlanarReflectionSettings.z > 0.5f)
    {
        float PlaneDistance = dot(
            MVOut.WorldPosition.xyz - PlanarReflectionMirrorPlanePosition.xyz,
            normalize(PlanarReflectionMirrorPlaneNormal.xyz));
        float ClipSign = PlanarReflectionMirrorPlaneNormal.w != 0.0f ? PlanarReflectionMirrorPlaneNormal.w : 1.0f;
        clip(PlaneDistance * ClipSign - 0.001f);
    }

    bool UseGlass = MatConstBuffer.UseGlass > 0.5f;
    MVOut.TexPositionHome /= MVOut.TexPositionHome.w;
    float AmbientAccessibility = SimpleSSAOMap.Sample(TextureSampler, MVOut.TexPositionHome.xy, 0.0f).r;

    // 杩斿洖闃村奖璐村浘閲囨牱
    // 銈枫儯銉夈偊銉炪儍銉椼偟銉炽儣銉兂銈般倰杩斻仚
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
        // 鍙嶅皠
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

	//銆€鑾峰彇娉曠嚎 濡傛灉璁剧疆浜嗘硶绾胯创鍥惧氨鑾峰彇娉曠嚎璐村浘
    //銆€娉曠窔銈掑彇寰?娉曠窔銉炪儍銉椼亴瑷畾銇曘倢銇︺亜銈嬪牬鍚堛伅娉曠窔銉炪儍銉椼倰鍙栧緱
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

			//銆€鍏颁集鐗?
            //銆€銉┿兂銉愩兗銉?
            if (MatConstBuffer.MaterialType == 0)
            {
                DotValue = pow(max(dot(ModelNormal, NormalizeLightDirection), 0.0), 2.f);
            }
			//銆€鍗婂叞浼壒
            //銆€銉忋兗銉曘儵銉炽儛銉笺儓
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

				// 鍏堝崐鍏板崥鐗瑰寲 鍐嶅噺鍘?.2f鏇濆厜鐨勶紝鍐嶅钩鏂癸紝鍙樺緱鏇存煍鍜?
                // 銇俱仛銉忋兗銉曘儵銉炽儛銉笺儓鍖栥仐銆佹銇?.2f銇湶鍑恒倰娓涚畻銇椼€併仌銈夈伀浜屼箺銇椼仸銆併倛銈婃煍銈夈亱銇忋仚銈?
                DotValue = pow(max(0.0, (dot(ModelNormal, NormalizeLightDirection) * 0.5f + 0.5f) - 0.2f), 2);

                float MaterialShininess = 1.f - saturate(MatConstBuffer.MaterialRoughness);
                float M = MaterialShininess * 100.f;

				// c=(m+2.f/PI) 褰掍竴鍖栫郴鏁?鍚庨潰浼氳缁嗚瑙ｆ帹瀵?
                // c=(m+2.f/PI) 姝ｈ鍖栦總鏁?寰屻仹瑭崇窗銇皫鍑恒倰瑾槑
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
					// 铻嶅叆鍗婂叞浼壒
                    // 銉忋兗銉曘儵銉炽儛銉笺儓銈掔祫銇胯炯銈€
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

					// 鐏厜鐐逛箻鍊?
                    // 銉┿偆銉堛伄鍐呯鍊?
                    float LightDotValue = dot(ModelNormal, NormalizeLightDirection);

                    float DiffuseReflection = (LightDotValue + 1.f) * 0.5f;

                    float Layered = 7.f;
                    DotValue = floor(DiffuseReflection * Layered) / Layered;

                    Material.BaseColor = lerp(Color2, Material.BaseColor, LightDotValue);
                }
            }
            //鏈€绲侭anded
            else if (MatConstBuffer.MaterialType == 8)//閺堚偓缁蹭经anded
            {
                if (i == 0)
                {
					//銆€铻嶅叆鍗婂叞浼壒
                    //銆€銉忋兗銉曘儵銉炽儛銉笺儓銈掔祫銇胯炯銈€
                    float DiffuseReflection = (dot(ModelNormal, NormalizeLightDirection) + 1.f) * 0.5f;

                    float Layered = 4.f;
                    DotValue = floor(DiffuseReflection * Layered) / Layered;

					//銆€鑿插凹灏旀晥鏋?
                    //銆€銉曘儸銉嶃儷鍔规灉
                    float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
                    float3 F0 = { 0.05f, 0.05f, 0.05f };
                    Specular.xyz = FresnelSchlickMethod(F0, ModelNormal, ViewDirection, 3).xyz;

					//銆€鍙嶅皠
                    float3 ReflectDirection = normalize(-reflect(NormalizeLightDirection, ModelNormal));

					//銆€highlight
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

				// 妯℃嫙閫忓皠鏁堟灉
                // 閫忛亷鍔规灉銈掋偡銉熴儱銉兗銉?
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

                float NormalLight = saturate(pow(max(dot(ModelNormal, NormalizeLightDirection), 0.0), 2.f)); //鍏颁集鐗?
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
            else if (MatConstBuffer.MaterialType == 15)//閫忔槑
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
			// 鑿插凹灏?
            // 銉曘儸銉嶃儷
            else if (MatConstBuffer.MaterialType == 100)
            {
                float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
                DotValue = pow(1.f - max(dot(ModelNormal, ViewDirection), 0.0), 2.f);

				// Schlick 鑿插凹灏旀柟娉?
                // Schlick 銉曘儸銉嶃儷
				//float3 F0 = { 0.1f,0.1f,0.1f };
				//Specular.xyz = FresnelSchlickMethod(F0, ModelNormal, ViewDirection, 3).xyz;
            }

            // 婕弽灏?
            // 鎷℃暎鍙嶅皠
            float4 Diffuse = Material.BaseColor;
            // highlight
			Specular = saturate(Specular);

            // shadow
            float ShadowFactor = 1.f;
			if (SceneLights[i].LightType == 1)
			{
                // 鐐瑰厜婧恈ubemap闃村奖
                // 鐐瑰厜婧愩伄cubemap銈枫儯銉夈偊
				ShadowFactor = ProcessingOmnidirectionalSampleCmpLevelZeroCubeMapShadow(MVOut.WorldPosition, SceneLights[i].Position);
			}
			else
			{
                // 姝ゅ鍙敤ShadowFunction涓殑澶氱鏂瑰紡璁＄畻闃村奖
                // 銇撱亾銇с伅ShadowFunction鍐呫伄瑜囨暟銇柟娉曘仹銈枫儯銉夈偊銈掕▓绠楀彲鑳?
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

    // 鏈€缁堥鑹?
    // 鏈€绲傝壊
    MVOut.Color = (FinalColor + Ambient) * AOSurfaceFactor;


    switch (MatConstBuffer.MaterialType)
    {
        case 2:
        case 3:
        case 9:
            {
			    //璁＄畻鍙嶅皠
                //鍙嶅皠銈掕▓绠?
                float3 ReflectionColor = GetReflectionColor(MatConstBuffer, ModelNormal, MVOut.WorldPosition.xyz);
                MVOut.Color.xyz += ReflectionColor;
                break;
            }
        // 閫忔槑绠楁硶
        // 閫忔槑銈儷銈淬儶銈恒儬
        case 15:
            {
			    // 璁＄畻鎶樺皠
                // 灞堟姌銈掕▓绠?
                float3 NewRefract = GetRefract(ModelNormal, MVOut.WorldPosition.xyz, MatConstBuffer.Refraction);
                float3 SampleRefractColor = GetReflectionSampleColor(MatConstBuffer, ModelNormal, MVOut.WorldPosition.xyz, NewRefract);

			    // 鍙嶅皠銈掕▓绠?
                float3 NewReflect = GetReflect(ModelNormal, MVOut.WorldPosition.xyz);
                float3 SampleReflectionColor = GetReflectionSampleColor(MatConstBuffer, ModelNormal, MVOut.WorldPosition.xyz, NewReflect);

			    // 璁＄畻A閫氶亾
                // A銉併儯銉炽儘銉倰瑷堢畻
                float3 V = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);
                float Shininess = GetShininess(MatConstBuffer);
                float3 FresnelFactor = FresnelSchlickFactor(MatConstBuffer, ModelNormal, V);

                float3 Color = lerp(SampleRefractColor, SampleReflectionColor, pow(Shininess * FresnelFactor, 2));

                MVOut.Color.xyz += Color;
                break;
            }
    }

    if (UseGlass || MatConstBuffer.MaterialType == 15)
    {
		// 閫忔槑
        MVOut.Color.a = MatConstBuffer.Transparency;
    }
    else
    {
		// 闈為€忔槑
        MVOut.Color.a = Material.BaseColor.a;
    }


	// 璁＄畻銉曘偐銈?
    MVOut.Color = GetFogValue(MVOut.Color, (float3)MVOut.WorldPosition);

    return MVOut.Color;
}
