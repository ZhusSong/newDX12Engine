#include "AOFunctionLibrary.hlsli"

static const float GContactStrength = 1.5f;
static const float GContactBias = 0.02f;

struct MeshVertexOut
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

MeshVertexOut VertexShaderMain(uint VertexID : SV_VertexID)
{
    MeshVertexOut Out = (MeshVertexOut)0.f;

    Out.TexCoord = TextureCoordinates[VertexID];
    Out.Position = float4(2.f * Out.TexCoord.x - 1.f, 1.f - 2.f * Out.TexCoord.y, 0.f, 1.f);

    return Out;
}

float4 PixelShaderMain(MeshVertexOut MVOut) : SV_TARGET
{
    float3 CenterNormal = normalize(SampleNormalMap.SampleLevel(TextureSampler, MVOut.TexCoord, 0.0f).xyz * 2.0f - 1.0f);
    float CenterDepth = SampleDepthMap.SampleLevel(TextureSampler, MVOut.TexCoord, 0.0f).r;
    float4 CenterClip = float4(2.0f * MVOut.TexCoord.x - 1.0f, 1.0f - 2.0f * MVOut.TexCoord.y, CenterDepth, 1.0f);
    float4 CenterView = mul(CenterClip, InversiveProjectionMatrix);
    CenterView /= max(CenterView.w, 1e-4f);
    float3 CenterViewPosition = CenterView.xyz;

    float3 NoiseValue = SampleNoiseMap.SampleLevel(TextureSampler, MVOut.TexCoord * 4.0f, 0.0f).xyz * 2.0f - 1.0f;
    float3 Tangent = normalize(NoiseValue - CenterNormal * dot(NoiseValue, CenterNormal));
    float3 Bitangent = normalize(cross(CenterNormal, Tangent));
    float3x3 TBN = float3x3(Tangent, Bitangent, CenterNormal);

    float Occlusion = 0.0f;
    float ValidSamples = 0.0f;

    [unroll]
    for (int i = 0; i < SAMPLE_VOLUME_NUM; ++i)
    {
        float3 SampleKernel = normalize(SampleVolumeBuffer[i].xyz);
        float RadiusScale = lerp(0.15f, 1.0f, SampleVolumeBuffer[i].w);
        float3 SampleOffset = mul(SampleKernel, TBN) * (OcclusionRadius * RadiusScale);
        float3 SampleViewPosition = CenterViewPosition + SampleOffset;

        float4 SampleClip = mul(float4(SampleViewPosition, 1.0f), ProjectionMatrix);
        if (abs(SampleClip.w) < 1e-4f)
        {
            continue;
        }

        SampleClip.xyz /= SampleClip.w;
        float2 SampleUV = float2(SampleClip.x * 0.5f + 0.5f, 0.5f - SampleClip.y * 0.5f);

        if (any(SampleUV < 0.0f) || any(SampleUV > 1.0f))
        {
            continue;
        }

        float SceneDepth = SampleDepthMap.SampleLevel(TextureSampler, SampleUV, 0.0f).r;
        float4 SceneClip = float4(2.0f * SampleUV.x - 1.0f, 1.0f - 2.0f * SampleUV.y, SceneDepth, 1.0f);
        float4 SceneView = mul(SceneClip, InversiveProjectionMatrix);
        SceneView /= max(SceneView.w, 1e-4f);
        float3 SceneViewPosition = SceneView.xyz;

        float DepthDelta = SceneViewPosition.z - SampleViewPosition.z;
        float SceneIsCloser = step(GContactBias, DepthDelta);
        float RangeWeight = smoothstep(1.0f, 0.0f, length(SceneViewPosition - CenterViewPosition) / max(OcclusionRadius * 1.10f, 1e-4f));
        float3 SampleNormal = normalize(SampleNormalMap.SampleLevel(TextureSampler, SampleUV, 0.0f).xyz * 2.0f - 1.0f);
        float NormalDifference = 1.0f - saturate(dot(CenterNormal, SampleNormal));
        float NormalWeight = smoothstep(0.04f, 0.28f, NormalDifference);
        float HemisphereWeight = saturate(dot(CenterNormal, normalize(SceneViewPosition - CenterViewPosition)) * 0.5f + 0.5f);
        float ContactDepthWeight = smoothstep(GContactBias, GContactBias + 0.04f, DepthDelta);

        Occlusion += SceneIsCloser * ContactDepthWeight * RangeWeight * max(NormalWeight, HemisphereWeight * 0.35f);
        ValidSamples += 1.0f;
    }

    Occlusion = saturate(Occlusion / max(ValidSamples, 1.0f));
    Occlusion = pow(saturate(Occlusion), 1.25f);
    Occlusion = saturate(Occlusion * GContactStrength);

    float Accessibility = 1.0f - Occlusion * 0.82f;
    Accessibility = saturate(max(Accessibility, 0.60f));

    return float4(Accessibility, Accessibility, Accessibility, 1.0f);
}
