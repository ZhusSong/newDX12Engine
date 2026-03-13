#include "AOFunctionLibrary.hlsli"

struct MeshVertexOut
{
    float4 ViewPosition : POSITION;
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

MeshVertexOut VertexShaderMain(uint VertexID : SV_VertexID)
{
    MeshVertexOut Out = (MeshVertexOut) 0.f;

    Out.TexCoord = TextureCoordinates[VertexID];

	// 映射到NDC空间
    // NDC空間にマッピング
    Out.Position = float4(2.f * Out.TexCoord.x - 1.f, 1.f - 2.f * Out.TexCoord.y, 0.f, 1.f);
	
    float4 PositionH = mul(Out.Position, InversiveProjectionMatrix); //视口空间 //ビューポート空間
    Out.ViewPosition.xyz = PositionH.xyz / PositionH.w; //近剪裁面  //近クリップ面

    return Out;
}
float4 PixelShaderMain(MeshVertexOut MVOut) : SV_TARGET
{
	// 获取当前像素的法线
    // 現在のピクセルの法線を取得
    float3 N = SampleNormalMap.SampleLevel(TextureSampler, MVOut.TexCoord, 0.0).xyz;
    float DepthNDC = SampleDepthMap.SampleLevel(DepthSampler, MVOut.TexCoord, 0.0).r;
	// 把深度从NDC空间转换到视口空间深度
    // 深度をNDC空間からビューポート空間深度に変換
    float AViewSpaceDepth = DepthNdcSpaceToViewSpace(DepthNDC);

	// 视口空间下当前像素的位置
    // ビューポート空間における現在のピクセルの位置
    float3 AViewSpacePosition = (AViewSpaceDepth / MVOut.ViewPosition.z) * MVOut.ViewPosition;

	// 环境光方向，计算颗粒度
    // 環境光の方向、粒度を計算
    float3 AmbientLightDirection = SampleNoiseMap.SampleLevel(TextureSampler, 4.f * MVOut.TexCoord, 0.0f);
	
    AmbientLightDirection = AmbientLightDirection.rgb * 2.f - 1.f;
    
    float OcclusionValue = 0.f;
	// 遍历采样体积中的多个方向
    // サンプリングボリューム内の複数の方向を反復処理
    for (int i = 0; i < SAMPLE_VOLUME_NUM; i++)
    {
		// 环境光反射
        // 環境光反射
        float3 AmbientLightReflect = reflect(SampleVolumeBuffer[i].xyz, AmbientLightDirection);

		// 当前采样点在发现的哪一侧
        // 現在のサンプルポイントが法線のどちら側にあるか
        float SignValue = sign(dot(AmbientLightReflect, N));

		// 沿着方向在OcclusionRadius半径内找到一个采样点
        // 方向に沿ってOcclusionRadius半径内のサンプルポイントを見つける
        float3 BViewSpacePosition = AViewSpacePosition + SignValue * AmbientLightReflect * OcclusionRadius;
		
		// 将找到的采样点转到投影空间，并计算出NDC坐标
        // 見つかったサンプルポイントを投影空間に変換し、NDC座標を計算
        float4 CTexProjectionSpace = mul(float4(BViewSpacePosition, 1.0f), TexProjectionMatrix);
        CTexProjectionSpace /= CTexProjectionSpace.w;

        float CDepthNDC = SampleDepthMap.SampleLevel(DepthSampler, CTexProjectionSpace.xy, 0.0).r;
	
		// 转为视口空间深度
        // ビューポート空間深度に変換
        float CViewDepth = DepthNdcSpaceToViewSpace(CDepthNDC);

		// 得到遮挡物在视口空间中的位置
        // 遮蔽物のビューポート空間における位置を取得
        float3 CViewSpacePosition = (CViewDepth / BViewSpacePosition.z) * BViewSpacePosition;

		// 点和遮挡物的距离
        // 点と遮蔽物の距離
        float DepthDistance = AViewSpacePosition.z - CViewSpacePosition.z;
        
        float NoAC = max(dot(N, normalize(CViewSpacePosition - AViewSpacePosition)), 0.f);

		// 根据深度差计算遮蔽强度
        // 深度差に基づいて遮蔽強度を計算
        OcclusionValue += NoAC * OcclusionFunction(DepthDistance);
    }
	// 求平均
    // 平均を求める
    OcclusionValue /= SAMPLE_VOLUME_NUM;
	// 最终可见性计算
    // 最終可視性の計算
    float Accessibility = 1.f - OcclusionValue;
    
    return saturate(pow(Accessibility, 6.0f));
}