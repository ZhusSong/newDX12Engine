#pragma once
#include "../../EngineMinimal.h"
#include "../../Math/EngineMath.h"

struct FVertex
{
	FVertex(const XMFLOAT3& InPos, const XMFLOAT4& InColor);


	FVertex(const XMFLOAT3& InPos, const XMFLOAT4& InColor, const XMFLOAT3& InNormal);

	XMFLOAT3 Position;
	XMFLOAT4 Color;
	XMFLOAT3 Normal;
};

struct FMeshRenderingData
{
	vector<FVertex> VertexData;
	vector<uint16_t> IndexData;
public:
	// 得到当前渲染对象顶点与索引size
	UINT GetVertexSizeInBytes() { return VertexData.size() * sizeof(FVertex); }
	UINT GetIndexSizeInBytes() { return IndexData.size() * sizeof(uint16_t); }
};

