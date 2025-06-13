#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"
#include "../../../../../Mesh/Core/MeshType.h"
#include "RenderingData.h"
#include "../DescriptorHeap/DirectXDescriptorHeap.h"
#include "../ConstantBuffer/ConstantBufferViews.h"
#include "../../../../../Core/Viewport/ViewportInfo.h"

class CMaterial;
struct FRenderingTexture;

// 几何体描述
struct FGeometry :public IDirectXDeviceInterface_Struct
{
	friend struct FGeometryMap;

	// 判断当前是否存在渲染数据
	bool IsRenderingDataExistence(CMeshComponent* InKey);

	// 通过hash值构建模型
	void BuildMesh(const size_t InMeshHash, CMeshComponent* InMesh, const FMeshRenderingData& MeshData, int InKey);
	void DuplicateMesh(CMeshComponent* InMesh, const FRenderingData& MeshData, int InKey);
	bool FindMeshRenderingDataByHash(const size_t& InHash, FRenderingData& MeshData, int InRenderLayerIndex = -1);

	void Build();

	UINT GetDrawObjectNumber() const;

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
	//vector<FRenderingData> DescribeMeshRenderingData;
};


//提供渲染内容的接口
struct FGeometryMap :public IDirectXDeviceInterface_Struct
{
	friend class FRenderLayer;

	FGeometryMap();
	~FGeometryMap();

	void PreDraw(float DeltaTime);
	void Draw(float DeltaTime);
	void PostDraw(float DeltaTime);


	void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	void UpdateMaterialShaderResourceView(float DeltaTime, const FViewportInfo& ViewportInfo);


	void BuildMesh(const size_t InMeshHash, CMeshComponent* InMesh, const FMeshRenderingData& MeshData);
	void DuplicateMesh(CMeshComponent* InMesh, const FRenderingData& MeshData);

	bool FindMeshRenderingDataByHash(const size_t& InHash, FRenderingData& MeshData, int InRenderLayerIndex = -1);

	void LoadTexture();

	// 构建模型
	void Build();

	// 描述堆
	void BuildDescriptorHeap();

	//构建常量缓冲区
	void BuildMeshConstantBuffer();

	//构建Material常量缓冲区
	void BuildMaterialShaderResourceView();

	//构建Light常量缓冲区
	void BuildLightConstantBuffer();

	// 得到绘制mesh对象的数量
	UINT GetDrawMeshObjectNumber();

	// 得到绘制mat材质对象的数量
	UINT GetDrawMaterialObjectNumber();

	// 得到绘制light灯光对象的数量
	UINT GetDrawLightObjectNumber();

	// 得到Texture资源数量
	UINT GetDrawTexture2DResourcesNumber();


	UINT GetDrawCubeMapResourcesNumber();

	// 构建纹理SRV视图
	void BuildTextureConstantBuffer();

	//构建视口常量缓冲区视图
	void BuildViewportConstantBufferView();


public:
	std::unique_ptr<FRenderingTexture>* FindRenderingTexture(const std::string& InKey);

public:
	void DrawLight(float DeltaTime);
	void DrawViewport(float DeltaTime);
	void DrawMesh(float DeltaTime);
	void DrawMaterial(float DeltaTime);
	void DrawTexture(float DeltaTime);
public:
	ID3D12DescriptorHeap* GetHeap()const { return DescriptorHeap.GetHeap(); }

protected:
	map<int, FGeometry> Geometrys;
	FDirectXDescriptorHeap DescriptorHeap;

	FConstantBufferViews MeshConstantBufferViews;
	FConstantBufferViews MaterialConstantBufferViews;
	FConstantBufferViews LightConstantBufferViews;
	FConstantBufferViews ViewportConstantBufferViews;

	std::shared_ptr<class FRenderingTextureResourcesUpdate> RenderingTexture2DResources;
	std::shared_ptr<class FRenderingTextureResourcesUpdate> RenderingCubeMapResources;
	std::vector<CMaterial*> Materials;
};
