#include "ShaderCommon.hlsli"
#include "Fog.hlsli"

struct MeshVertexIn
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};


struct MeshVertexOut
{
	float4 PositionH : POSITION;
	float4 PositionL : SV_POSITION;
};

MeshVertexOut VertexShaderMain(MeshVertexIn MV)
{
	MeshVertexOut Out = (MeshVertexOut)0;
	
	Out.PositionH = float4(MV.Position, 1.f);

	// 世界坐标
	// ワールド座標
	float4 WorldPosition = mul(Out.PositionH, WorldMatrix);

	// 变换到齐次剪裁空间
	// 同次クリップ空間に変換
	Out.PositionL = mul(WorldPosition, ViewProjectionMatrix);

	return Out;
}

float4 PixelShaderMain(MeshVertexOut MVOut) :SV_TARGET
{
    float4 Color = SimpleCubeMap.Sample(TextureSampler, (float3)MVOut.PositionH);

	float4 WorldPosition = mul(MVOut.PositionH, WorldMatrix);
    Color = GetFogValue(Color, (float3) WorldPosition);

	return Color;
}