//25.7.10 李
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
	float2 TexCoord  : TEXCOORD;
};

MeshVertexOut VertexShaderMain(MeshVertexIn MV)
{
	MaterialConstBuffer MatConstBuffer = Materials[MaterialIndex];

	MeshVertexOut Out = (MeshVertexOut)0;

	float4 PositionWorld = mul(float4(MV.Position, 1.0f), WorldMatrix);
	Out.PositionH = mul(PositionWorld, ViewProjectionMatrix);

	float4 MyTexCoord = mul(float4(MV.TexCoord, 0.0f, 1.f), ObjectTextureTransform);
	Out.TexCoord = mul(MyTexCoord, MatConstBuffer.TransformInformation).xy;

	return Out;
}

float4 PixelShaderMain(MeshVertexOut MVOut) :SV_TARGET
{
	return float4(1.f,0.f, 0.f,1.f);
}