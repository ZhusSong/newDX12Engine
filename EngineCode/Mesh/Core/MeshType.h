#pragma once
#include "../../EngineMinimal.h"
#include "../../Math/EngineMath.h"

struct FVertex
{
	FVertex(const XMFLOAT3& InPos, const XMFLOAT4& InColor);

	FVertex(const XMFLOAT3& InPos, const XMFLOAT4& InColor, const XMFLOAT3& InNormal, const XMFLOAT2& InTexCoord = XMFLOAT2(0.f, 0.f));

	XMFLOAT3 Position;
	XMFLOAT4 Color;
	XMFLOAT3 Normal;
	XMFLOAT3 UTangent;
	XMFLOAT2 TexCoord; // 纹理坐标
};

struct FMeshRenderingData
{
	vector<FVertex> VertexData;
	vector<uint16_t> IndexData;
public:
	// 得到当前渲染对象顶点与索引size
	UINT GetVertexSizeInBytes() { return  (UINT)VertexData.size() * sizeof(FVertex); }
	UINT GetIndexSizeInBytes() { return  (UINT)IndexData.size() * sizeof(uint16_t); }
};

enum EPyramidNumberSides
{
	Pyramid_3 = 3,
	Pyramid_4,
	Pyramid_5,
};