#pragma once
#include "../../../../../Interface/DirectXDeviceInterfece.h"
#include "../../../../../Mesh/Core/MeshType.h"
#include "RenderingData.h"
#include "../DescriptorHeap/DirectXDescriptorHeap.h"
#include "../ConstantBuffer/ConstantBufferViews.h"
#include "../../../../../Core/Viewport/ViewportInfo.h"

// 几何体描述
struct FGeometry :public IDirectXDeviceInterface_Struct
{
	friend struct FGeometryMap;

	// 判断当前是否存在渲染数据
	bool bRenderingDataExistence(GMesh* InKey);
	void BuildMesh(GMesh* InMesh, const FMeshRenderingData& MeshData);

	void Build();

	UINT GetDrawObjectNumber() const { return DescribeMeshRenderingData.size(); }

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
protected:
	ComPtr<ID3DBlob> CPUVertexBufferPtr;
	ComPtr<ID3DBlob> CPUIndexBufferPtr;

	ComPtr<ID3D12Resource> GPUVertexBufferPtr;
	ComPtr<ID3D12Resource> GPUIndexBufferPtr;

	ComPtr<ID3D12Resource> VertexBufferTmpPtr;
	ComPtr<ID3D12Resource> IndexBufferTmpPtr;

	FMeshRenderingData MeshRenderingData;

	// 渲染数据描述列表
	vector<FRenderingData> DescribeMeshRenderingData;
};
struct FGeometryMap :public IDirectXDeviceInterface_Struct
{
	FGeometryMap();

	void PreDraw(float DeltaTime);
	void Draw(float DeltaTime);
	void PostDraw(float DeltaTime);

	void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	void BuildMesh(GMesh* InMesh, const FMeshRenderingData& MeshData);

	void Build();
	void BuildDescriptorHeap();

	void BuildConstantBuffer();

	UINT GetDrawObjectNumber();

	void BuildViewportConstantBufferView();

public:
	void DrawViewport(float DeltaTime);
	void DrawMesh(float DeltaTime);
public:
	ID3D12DescriptorHeap* GetHeap()const { return DescriptorHeap.GetHeap(); }

protected:
	map<int, FGeometry> Geometrys;
	FDirectXDescriptorHeap DescriptorHeap;

	FConstantBufferViews ObjectConstantBufferViews;
	FConstantBufferViews ViewportConstantBufferViews;

};
