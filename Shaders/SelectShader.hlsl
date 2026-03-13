#include "ShaderCommon.hlsli"
#include "Material.hlsli"

struct MeshVertexIn
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct MeshVertexOut
{
	float4 WorldPosition : POSITION;
	float4 PositionH : SV_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord  : TEXCOORD;
};

MeshVertexOut VertexShaderMain(MeshVertexIn MV)
{
	//MaterialConstBuffer MatConstBuffer = Materials[MaterialIndex];

	//MeshVertexOut Out = (MeshVertexOut)0;

	//Out.WorldPosition = mul(float4(MV.Position, 1.0f), WorldMatrix);

	//float3 ViewDirection = normalize(ViewportPosition.xyz - Out.WorldPosition.xyz);
	//Out.WorldPosition.xyz += ViewDirection;
	////PositionWorld.x += 4.f; Debug

	//Out.PositionH = mul(Out.WorldPosition, ViewProjectionMatrix);
	
	//float4 MyTexCoord = mul(float4(MV.TexCoord, 0.0f, 1.f), ObjectTextureTransform);
	//Out.TexCoord = mul(MyTexCoord, MatConstBuffer.TransformInformation).xy;
	
	//Out.Normal = mul(MV.Normal, (float3x3)NormalTransformation);

	//return Out;
	
	// 新描边
	// 新しいアウトライン
    MaterialConstBuffer MatConstBuffer = Materials[MaterialIndex];
    MeshVertexOut Out = (MeshVertexOut) 0;

    // 计算世界空间位置
	// ワールド空間位置を計算
    Out.WorldPosition = mul(float4(MV.Position, 1.0f), WorldMatrix);
    
    // 计算世界空间法线
	// ワールド空間法線を計算
    float3 WorldNormal = normalize(mul(MV.Normal, (float3x3) NormalTransformation));
    
    // 沿法线方向外扩，创建描边效果
	// 法線方向に沿って外側に拡張し、アウトライン効果を作成
    float OutlineWidth = 0.05f; // 描边宽度  // アウトラインの幅
    Out.WorldPosition.xyz += WorldNormal * OutlineWidth;
    
    // 变换到裁剪空间
	// クリップ空間に変換
    Out.PositionH = mul(Out.WorldPosition, ViewProjectionMatrix);

    // UV坐标变换
	// UV座標変換
    float4 MyTexCoord = mul(float4(MV.TexCoord, 0.0f, 1.f), ObjectTextureTransform);
    Out.TexCoord = mul(MyTexCoord, MatConstBuffer.TransformInformation).xy;
	
	// 法線
    Out.Normal = WorldNormal;

    return Out;
}

float4 PixelShaderMain(MeshVertexOut MVOut) :SV_TARGET
{
	//float3 ModelNormal = normalize(MVOut.Normal);

	//float3 ViewDirection = normalize(ViewportPosition.xyz - MVOut.WorldPosition.xyz);

	//// 菲尼尔式描边
	////float DotValue = pow(1.f - max(dot(ModelNormal, ViewDirection), 0.0), 3.f);
	////
	////return float4(1.f, 0.f, 0.f, 1.f) * DotValue;
	//float NoV = dot(ModelNormal, ViewDirection);
	//if (NoV <= 0.2f)
	//{
	//	return float4(1.f, 1.f, 0.f, 1.f);
	//}

	//return float4(0.f, 0.f, 0.f, 0.f);
	
	// 新描边
	// 新しいアウトライン
    return float4(1.f, 1.f, 0.f, 1.f); 
}