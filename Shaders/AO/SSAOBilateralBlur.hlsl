#include "AOFunctionLibrary.hlsli"

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
    float BlurWeights[12] =
    {
        Float4BlurWeights[0].x, Float4BlurWeights[0].y, Float4BlurWeights[0].z, Float4BlurWeights[0].w,
        Float4BlurWeights[1].x, Float4BlurWeights[1].y, Float4BlurWeights[1].z, Float4BlurWeights[1].w,
        Float4BlurWeights[2].x, Float4BlurWeights[2].y, Float4BlurWeights[2].z, Float4BlurWeights[2].w,
    };

    float2 TexOffset = (bHorizontalBlur != 0u)
        ? float2(InversionSize.x, 0.f)
        : float2(0.f, InversionSize.y);

    int Radius = (int)BlurRadius;
    float Weights = BlurWeights[Radius];
    float4 Color = Weights * SampleAcceptMap.SampleLevel(TextureSampler, MVOut.TexCoord, 0.0f);

    float4 MatchingNormalDepth = SampleNormalMap.SampleLevel(TextureSampler, MVOut.TexCoord, 0.0f);
    float3 MatchingNormal = MatchingNormalDepth.xyz;
    float MatchingViewDepth = DepthNdcSpaceToViewSpace(MatchingNormalDepth.w);

    for (int i = -Radius; i <= Radius; ++i)
    {
        if (i == 0)
        {
            continue;
        }

        float2 TexCoord = MVOut.TexCoord + i * TexOffset;
        float4 NormalDepth = SampleNormalMap.SampleLevel(TextureSampler, TexCoord, 0.0f);
        float3 Normal = NormalDepth.xyz;
        float ViewDepth = DepthNdcSpaceToViewSpace(NormalDepth.w);

        if (dot(Normal, MatchingNormal) >= 0.8f && abs(ViewDepth - MatchingViewDepth) <= 0.2f)
        {
            float Weight = BlurWeights[Radius + i];
            Color += Weight * SampleAcceptMap.SampleLevel(TextureSampler, TexCoord, 0.0f);
            Weights += Weight;
        }
    }

    return Color / max(Weights, 0.0001f);
}
