#pragma once
#include "../../../../../Interface/DirectXDeviceInterfece.h"
#include "../../../RenderingResourcesUpdate.h"

class GMesh;
// 渲染所需数据结构体
struct FRenderingData :public IDirectXDeviceInterface_Struct
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
	UINT GetVertexSizeInBytes()const { return VertexSize * VertexTypeSize; }
	UINT GetIndexSizeInBytes()const { return IndexSize * IndexTypeSize; }
public:
	DXGI_FORMAT IndexFormat;

	XMFLOAT4X4 WorldMatrix;

	GMesh* Mesh;

};