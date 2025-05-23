﻿#include "RenderingData.h"
#include "../../../../../Mesh/Core/MeshType.h"

// 渲染数据描述
FRenderingData::FRenderingData()
	: IndexSize(0)
	, VertexSize(0)
	, IndexOffsetPosition(0)
	, VertexOffsetPosition(0)
	, VertexTypeSize(sizeof(FVertex))
	, IndexTypeSize(sizeof(uint16_t))
	, IndexFormat(DXGI_FORMAT_R16_UINT)
	, WorldMatrix(EngineMath::IdentityMatrix4x4())
	, Mesh(NULL)
{

}