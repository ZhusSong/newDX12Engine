#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"
#include "../../../RenderingResourcesUpdate.h"

class CMeshComponent;
struct FMeshRenderingData;

// 渲染所需数据结构体
struct FRenderingData
	:public IDirectXDeviceInterface_Struct
	, public std::enable_shared_from_this<FRenderingData>
{
public:
	FRenderingData();

	// 渲染物体索引与顶点数据所需空间
	UINT IndexSize;
	UINT VertexSize;

	// 索引与顶点偏移
	UINT IndexOffsetPosition;
	UINT VertexOffsetPosition;

	UINT VertexTypeSize;
	UINT IndexTypeSize;


public:
	// AABB包围盒
	BoundingBox Bounds;


	size_t MeshHash;
	int GeometryKey;
	int MeshObjectIndex;

public:
	UINT GetVertexSizeInBytes()const { return VertexSize * VertexTypeSize; }
	UINT GetIndexSizeInBytes()const { return IndexSize * IndexTypeSize; }
public:
	DXGI_FORMAT IndexFormat;

	XMFLOAT4X4 WorldMatrix;

	// 纹理数据
	XMFLOAT4X4 TextureTransform;

	CMeshComponent* Mesh;

	FMeshRenderingData* MeshRenderingData;

};