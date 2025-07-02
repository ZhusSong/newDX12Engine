#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"
#include "../../../../../Mesh/Core/MeshType.h"
#include "RenderingData.h"
#include "../DescriptorHeap/DirectXDescriptorHeap.h"
#include "../ConstantBuffer/ConstantBufferViews.h"
#include "../../../../../Core/Viewport/ViewportInfo.h"
#include "../DynamicMap/ShadowMap/DynamicShadowMap.h"
#include "../DynamicMap/ShadowMap/DynamicShadowCubeMap.h"

class CMaterial;
struct FRenderingTexture;
class CFogComponent;

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
	friend class FDynamicReflectionCubeMap;
	friend class FDynamicShadowMap;
	friend class FRenderingPipeline;

	FGeometryMap();
	~FGeometryMap();

	void PreDraw(float DeltaTime);
	void Draw(float DeltaTime);
	void PostDraw(float DeltaTime);

	// 更新每帧运算
	void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	// 更新视口
	void UpdateCalculationsViewport(
		float DeltaTime,
		const FViewportInfo& ViewportInfo,
		UINT InConstantBufferOffset);


	// 更新材质
	void UpdateMaterialShaderResourceView(float DeltaTime, const FViewportInfo& ViewportInfo);
	
	// 更新灯光
	void UpdateLight(float DeltaTime, const FViewportInfo& ViewportInfo);
	
	// 更新雾
	void UpdateFog(float DeltaTime, const FViewportInfo& ViewportInfo);

	//收集动态反射模型
	void BuildDynamicReflectionMesh();
	void BuildFog();
	void BuildShadow();

	void BuildMesh(const size_t InMeshHash, CMeshComponent* InMesh, const FMeshRenderingData& MeshData);
	void DuplicateMesh(CMeshComponent* InMesh, const FRenderingData& MeshData);

	bool FindMeshRenderingDataByHash(const size_t& InHash, FRenderingData& MeshData, int InRenderLayerIndex = -1);

	// 读取Texture
	void LoadTexture();

	// 构建模型
	void Build();

	// 描述堆
	void BuildDescriptorHeap();

	//构建Mesh常量缓冲区
	void BuildMeshConstantBuffer();

	// 构建雾常量缓冲区
	void BuildFogConstantBuffer();

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


	// CubeMap贴图数量
	UINT GetDrawCubeMapResourcesNumber();


	//动态摄像机
	UINT GetDynamicReflectionViewportNum();

	// 构建纹理SRV视图
	void BuildTextureConstantBuffer();

	//构建我们的视口常量缓冲区视图
	void BuildViewportConstantBufferView(UINT InViewportOffset = 0);

public:
	// ShadowCubeMap相关
	// 获取动态反射模型组件
	UINT GetDynamicReflectionMeshComponentsSize();

	CMeshComponent* GetDynamicReflectionMeshComponents(int Index);

	// 获取视口常量缓冲区size
	UINT GetViewportConstantBufferByteSize();

	// 获取GPU访问资源时使用的虚拟地址
	D3D12_GPU_VIRTUAL_ADDRESS ViewportGPUVirtualAddress();

public:
	bool IsStartUPFog();

public:
	std::unique_ptr<FRenderingTexture>* FindRenderingTexture(const std::string& InKey);

public:
	void DrawShadow(float DeltaTime);
	void DrawLight(float DeltaTime);
	void DrawViewport(float DeltaTime);
	void DrawMesh(float DeltaTime);
	void DrawMaterial(float DeltaTime);
	void Draw2DTexture(float DeltaTime);

	void DrawCubeMapTexture(float DeltaTime);
	void DrawFog(float DeltaTime);
public:
	ID3D12DescriptorHeap* GetHeap()const { return DescriptorHeap.GetHeap(); }

protected:
	map<int, FGeometry> Geometrys;
	FDirectXDescriptorHeap DescriptorHeap;

	
	FConstantBufferViews MeshConstantBufferViews;		//网格常量缓冲区
	FConstantBufferViews MaterialConstantBufferViews;	//材质常量缓冲区
	FConstantBufferViews LightConstantBufferViews;		//灯光常量缓冲区
	FConstantBufferViews ViewportConstantBufferViews;	//视口常量缓冲区
	FConstantBufferViews FogConstantBufferViews;		//雾常量缓冲区

	std::shared_ptr<class FRenderingTextureResourcesUpdate> RenderingTexture2DResources;
	std::shared_ptr<class FRenderingTextureResourcesUpdate> RenderingCubeMapResources;
	std::vector<CMaterial*> Materials;
	std::vector<CMeshComponent*> DynamicReflectionMeshComponents;

	CFogComponent* Fog;

	FDynamicShadowMap DynamicShadowMap;

	FDynamicShadowCubeMap DynamicShadowCubeMap;
};
