#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"
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
	bool bRenderingDataExistence(CMeshComponent* InKey);
	void BuildMesh(CMeshComponent* InMesh, const FMeshRenderingData& MeshData);

	void Build();

	UINT GetDrawObjectNumber() const { return DescribeMeshRenderingData.size(); }

	// 得到顶点与索引缓冲区视图
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

	void BuildMesh(CMeshComponent* InMesh, const FMeshRenderingData& MeshData);

	// 构建模型
	void Build();

	// 描述堆
	void BuildDescriptorHeap();

	//构建常量缓冲区
	void BuildMeshConstantBuffer();

	//构建Material常量缓冲区
	void BuildMaterialConstantBuffer();

	//构建Light常量缓冲区
	void BuildLightConstantBuffer();

	//构建视口常量缓冲区视图
	void BuildViewportConstantBufferView();

	// 得到绘制mesh对象的数量
	UINT GetDrawMeshObjectNumber();

	// 得到绘制mat材质对象的数量
	UINT GetDrawMaterialObjectNumber();

	// 得到绘制light灯光对象的数量
	UINT GetDrawLightObjectNumber();


public:
	void DrawLight(float DeltaTime);
	void DrawViewport(float DeltaTime);
	void DrawMesh(float DeltaTime);
public:
	ID3D12DescriptorHeap* GetHeap()const { return DescriptorHeap.GetHeap(); }

protected:
	map<int, FGeometry> Geometrys;
	FDirectXDescriptorHeap DescriptorHeap;

	FConstantBufferViews MeshConstantBufferViews;
	FConstantBufferViews MaterialConstantBufferViews;
	FConstantBufferViews LightConstantBufferViews;
	FConstantBufferViews ViewportConstantBufferViews;

};
