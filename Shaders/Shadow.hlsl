#include "ShaderCommon.hlsli"
#include "Material.hlsli"

struct MeshVertexIn
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct MeshVertexOut
{
    float4 PositionH : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

MeshVertexOut VertexShaderMain(MeshVertexIn MV)
{
    MaterialConstBuffer MatConstBuffer = Materials[MaterialIndex];

    MeshVertexOut Out = (MeshVertexOut) 0;

    float4 PositionWorld = mul(float4(MV.Position, 1.0f), WorldMatrix);
    Out.PositionH = mul(PositionWorld, ViewProjectionMatrix);
    
    // UI座標
    float4 MyTexCoord = mul(float4(MV.TexCoord, 0.0f, 1.f), ObjectTextureTransform);
    Out.TexCoord = mul(MyTexCoord, MatConstBuffer.TransformInformation).xy;

    return Out;
}

void PixelShaderMain(MeshVertexOut MVOut)
{
    MaterialConstBuffer MatConstBuffer = Materials[MaterialIndex];
    float4 BaseColor = GetMaterialBaseColor(MatConstBuffer, MVOut.TexCoord);

    // Only discard fully transparent texels so thin alpha-mask details can still cast shadows.
    clip(BaseColor.a - (1.0f / 255.0f));
}
