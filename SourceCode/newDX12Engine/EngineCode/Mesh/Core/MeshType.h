#pragma once
#include "../../EngineMinimal.h"
#include "../../Math/EngineMath.h"

struct FVertex
{
	FVertex();
	FVertex(const XMFLOAT3& InPos, const XMFLOAT4& InColor);

	FVertex(const XMFLOAT3& InPos, const XMFLOAT4& InColor, const XMFLOAT3& InNormal, const XMFLOAT2& InTexCoord = XMFLOAT2(0.f, 0.f));

	XMFLOAT3 Position;
	XMFLOAT4 Color;
	XMFLOAT3 Normal;
	XMFLOAT3 UTangent;
	XMFLOAT2 TexCoord; // 纹理坐标  // テクスチャ座標
};

struct FMeshSection
{
	FMeshSection()
		: VertexOffset(0)
		, VertexSize(0)
		, IndexOffset(0)
		, IndexSize(0)
		, MaterialSlotIndex(0)
	{
	}

	UINT VertexOffset;
	UINT VertexSize;
	UINT IndexOffset;
	UINT IndexSize;
	UINT MaterialSlotIndex;
};

struct FMeshRenderingData
{
	vector<FVertex> VertexData;
	vector<uint16_t> IndexData;
	vector<FMeshSection> Sections;
public:
	// 得到当前渲染对象顶点与索引size
	// 現在のレンダリングオブジェクトの頂点およびインデックスサイズを取得
	UINT GetVertexSizeInBytes() { return  (UINT)VertexData.size() * sizeof(FVertex); }
	UINT GetIndexSizeInBytes() { return  (UINT)IndexData.size() * sizeof(uint16_t); }

	void BuildDefaultSectionIfNeeded()
	{
		if (!Sections.empty())
		{
			return;
		}

		FMeshSection Section;
		Section.VertexOffset = 0;
		Section.VertexSize = (UINT)VertexData.size();
		Section.IndexOffset = 0;
		Section.IndexSize = (UINT)IndexData.size();
		Section.MaterialSlotIndex = 0;
		Sections.push_back(Section);
	}
};

enum EPyramidNumberSides
{
	Pyramid_3 = 3,
	Pyramid_4,
	Pyramid_5,
};
