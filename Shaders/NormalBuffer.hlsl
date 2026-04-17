#include "Material.hlsli"

struct MeshVertexIn
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 UTangent : TANGENT;
    float2 TexCoord : TEXCOORD;
};

struct MeshVertexOut
{
    float4 WorldPosition : POSITION;
    float4 ClipPosition : POSITION1;
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float3 UTangent : TANGENT;
    float2 TexCoord : TEXCOORD;
};

struct NormalDepthOut
{
    float4 Normal : SV_TARGET0;
    float4 Depth : SV_TARGET1;
};

MeshVertexOut VertexShaderMain(MeshVertexIn MV)
{
    MaterialConstBuffer MatConstBuffer = Materials[MaterialIndex];

    MeshVertexOut Out = (MeshVertexOut) 0.f;


	// 世界坐标
    // ワールド座標
	Out.WorldPosition = mul(float4(MV.Position, 1.f), WorldMatrix);

	// 变换到齐次空间
    // 同次空間に変換
	Out.ClipPosition = mul(Out.WorldPosition, ViewProjectionMatrix);
	Out.Position = Out.ClipPosition;

	// 法线
    // 法線
	Out.Normal = mul(MV.Normal, (float3x3)WorldMatrix);

	// 切线
    // 接線
	Out.UTangent = mul(MV.UTangent, (float3x3)WorldMatrix);
    
    // UI座標
    float4 MyTexCoord = mul(float4(MV.TexCoord, 0.0f, 1.f), ObjectTextureTransform);
    Out.TexCoord = mul(MyTexCoord, MatConstBuffer.TransformInformation).xy;

	return Out;
}

NormalDepthOut PixelShaderMain(MeshVertexOut MVOut)
{
    NormalDepthOut Out = (NormalDepthOut)0.0f;
    float3 NormalizeWorldNormal = normalize(MVOut.Normal);
    // 独立深度输入：写入投影后的 NDC 深度，供 SSAO 在视空间内重建位置。
    float DepthNDC = saturate(MVOut.ClipPosition.z / max(MVOut.ClipPosition.w, 1e-4f));

    float3 EncodedNormal = NormalizeWorldNormal * 0.5f + 0.5f;
    Out.Normal = float4(EncodedNormal, 1.0f);
    Out.Depth = float4(DepthNDC, 1.0f, 1.0f, 1.0f);
    return Out;
}
