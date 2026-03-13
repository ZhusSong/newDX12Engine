#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"
#include "../../../RenderingResourcesUpdate.h"

class CMeshComponent;
struct FMeshRenderingData;

// 渲染所需数据结构体
// レンダリングに必要なデータ構造体
struct FRenderingData
	:public IDirectXDeviceInterface_Struct
	, public std::enable_shared_from_this<FRenderingData>
{
public:
	FRenderingData();

	// 渲染物体索引与顶点数据所需空间
	// 描画オブジェクトのインデックスおよび頂点データに必要な領域
	UINT IndexSize;
	UINT VertexSize;

	// 索引与顶点偏移
	// インデックスおよび頂点のオフセット
	UINT IndexOffsetPosition;
	UINT VertexOffsetPosition;

	UINT VertexTypeSize;
	UINT IndexTypeSize;


public:
	// AABB包围盒
	// AABBバウンディングボックス
	BoundingBox Bounds;


	size_t MeshHash;
	int GeometryKey;
	int MeshObjectIndex;

public:
	UINT GetVertexSizeInBytes()const { return VertexSize * VertexTypeSize; }
	UINT GetIndexSizeInBytes()const { return IndexSize * IndexTypeSize; }

	XMFLOAT4X4 GetWorldMatrix()const { return WorldMatrix; }
public:
	DXGI_FORMAT IndexFormat;

	XMFLOAT4X4 WorldMatrix;

	// 纹理数据
	// テクスチャデータ
	XMFLOAT4X4 TextureTransform;

	CMeshComponent* Mesh;

	FMeshRenderingData* MeshRenderingData;

};