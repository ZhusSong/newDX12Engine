#ifndef SHADERFUNCTIONLIBRARY_HLSL
#define SHADERFUNCTIONLIBRARY_HLSL

float3x3 GetBuildTBNMatrix(float3 InUnitWorldNormal, float3 InWorldTangent)
{
    float3 N = InUnitWorldNormal;

	// 矫正 万一原来的切线和法线不是垂直
    // 補正 元の接線と法線が垂直でない場合に備えて
    float3 T = normalize(InWorldTangent - dot(InWorldTangent, N) * N);

    float3 B = cross(N, T);

    return float3x3(T, B, N);
}

#endif