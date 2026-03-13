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

enum EFindValueType
{
	TYPE_IN_PROGRAM,
	TYPE_COMPLETE,
};

// 几何体描述
// 立体の説明
struct FGeometry :public IDirectXDeviceInterface_Struct
{
	friend struct FGeometryMap;

	// 判断当前是否存在渲染数据
	// 現在レンダリングデータが存在するかどうかを判定する
	bool IsRenderingDataExistence(CMeshComponent* InKey);

	// 通过hash值构建模型
	// ハッシュ値によってモデルを構築する
	void BuildMesh(const size_t InMeshHash, CMeshComponent* InMesh, const FMeshRenderingData& MeshData, int InKey);
	
	// 复制模型数据
	// モデルデータを複製する
	void DuplicateMesh(CMeshComponent* InMesh, std::shared_ptr<FRenderingData>& MeshData, int InKey);

	// 通过hash值查找渲染数据
	// ハッシュ値によってレンダリングデータを検索する
	bool FindMeshRenderingDataByHash(const size_t& InHash, std::shared_ptr<FRenderingData>& MeshData, int InRenderLayerIndex = -1);

	//构建模型
	// モデルを構築する
	void Build();

	UINT GetDrawObjectNumber() const;

	// 得到顶点与索引缓冲区视图
	// 頂点バッファおよびインデックスバッファのビューを取得する
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();

public:
	static void FindRenderingDatas(std::function<EFindValueType(std::shared_ptr<FRenderingData>&)> InFun);

protected:
	ComPtr<ID3DBlob> CPUVertexBufferPtr;
	ComPtr<ID3DBlob> CPUIndexBufferPtr;

	ComPtr<ID3D12Resource> GPUVertexBufferPtr;
	ComPtr<ID3D12Resource> GPUIndexBufferPtr;

	ComPtr<ID3D12Resource> VertexBufferTmpPtr;
	ComPtr<ID3D12Resource> IndexBufferTmpPtr;

	// 渲染数据
	// レンダリングデータ
	FMeshRenderingData MeshRenderingData;

protected:
	// 渲染池单例
	// レンダリングプールのシングルトン
	static map<size_t, std::shared_ptr<FRenderingData>> UniqueRenderingDatas;

public:
	// 实际使用的渲染池 里面会有重复的 key (size_t)
	// 実際に使用されるレンダリングプール。ここには重複したキー（size_t）が含まれる
	static vector<std::shared_ptr<FRenderingData>> RenderingDatas;
};

//提供渲染内容的接口
// レンダリング内容を提供するインターフェース
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
	virtual void OnResetSize(int InWidth, int InHeight);

	// 更新每帧运算
	// 毎フレームの計算を更新する
	void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	// 更新视口
	// ビューポートを更新する
	void UpdateCalculationsViewport(
		float DeltaTime,
		const FViewportInfo& ViewportInfo,
		UINT InConstantBufferOffset);


	// 更新材质
	// マテリアルを更新する
	void UpdateMaterialShaderResourceView(float DeltaTime, const FViewportInfo& ViewportInfo);
	
	// 更新灯光
	// ライトを更新する
	void UpdateLight(float DeltaTime, const FViewportInfo& ViewportInfo);
	
	// 更新雾
	// フォグを更新する
	void UpdateFog(float DeltaTime, const FViewportInfo& ViewportInfo);

	//收集动态反射模型
	// 動的反射モデルを収集する
	void BuildDynamicReflectionMesh();
	void BuildFog();
	void BuildShadow();

	void BuildMesh(const size_t InMeshHash, CMeshComponent* InMesh, const FMeshRenderingData& MeshData);
	void DuplicateMesh(CMeshComponent* InMesh, std::shared_ptr<FRenderingData>& MeshData);
	bool FindMeshRenderingDataByHash(const size_t& InHash, std::shared_ptr<FRenderingData>& MeshData, int InRenderLayerIndex = -1);

	// 读取Texture
	// テクスチャを読み込む
	void LoadTexture();

	// 构建模型
	// モデルを構築する
	void Build();

	// 描述堆
	// ディスクリプタヒープを構築する
	void BuildDescriptorHeap();

	//构建Mesh常量缓冲区
	// メッシュの定数バッファを構築する
	void BuildMeshConstantBuffer();

	// 构建雾常量缓冲区
	// フォグの定数バッファを構築する
	void BuildFogConstantBuffer();

	//构建Material常量缓冲区
	// マテリアルの定数バッファを構築する
	void BuildMaterialShaderResourceView();

	//构建Light常量缓冲区
	// ライトの定数バッファを構築する
	void BuildLightConstantBuffer();

	// 得到绘制mesh对象的数量
	// 描画するメッシュオブジェクトの数を取得する
	UINT GetDrawMeshObjectNumber();

	// 得到绘制mat材质对象的数量
	// 描画するマテリアルオブジェクトの数を取得する
	UINT GetDrawMaterialObjectNumber();

	// 得到绘制light灯光对象的数量
	// 描画するライトオブジェクトの数を取得する
	UINT GetDrawLightObjectNumber();

	// 得到Texture资源数量
	// テクスチャリソースの数を取得する
	UINT GetDrawTexture2DResourcesNumber();


	// CubeMap贴图数量
	// キューブマップテクスチャの数
	UINT GetDrawCubeMapResourcesNumber();


	//动态摄像机
	// 動的カメラの数
	UINT GetDynamicReflectionViewportNum();

	// 构建纹理SRV视图
	// テクスチャSRVビューを構築する
	void BuildTextureConstantBuffer();

	//构建视口常量缓冲区视图
	// ビューポート定数バッファビューを構築する
	void BuildViewportConstantBufferView(UINT InViewportOffset = 0);

public:
	// ShadowCubeMap相关
	// 获取动态反射模型组件
	// ShadowCubeMapに関する
	// 動的反射モデルコンポーネントを取得する
	UINT GetDynamicReflectionMeshComponentsSize();

	CMeshComponent* GetDynamicReflectionMeshComponents(int Index);

	// 获取视口常量缓冲区size
	// ビューポート定数バッファのサイズを取得する
	UINT GetViewportConstantBufferByteSize();

	// 获取GPU访问资源时使用的虚拟地址
	// GPUがリソースにアクセスする際に使用する仮想アドレスを取得する
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

	// 网格常量缓冲区
	// メッシュ定数バッファ
	FConstantBufferViews MeshConstantBufferViews;

	// 材质常量缓冲区
	// マテリアル定数バッファ
	FConstantBufferViews MaterialConstantBufferViews;

	// 灯光常量缓冲区
	// ライト定数バッファ
	FConstantBufferViews LightConstantBufferViews;

	// 视口常量缓冲区
	// ビューポート定数バッファ
	FConstantBufferViews ViewportConstantBufferViews;

	// 雾常量缓冲区
	// フォグ定数バッファ
	FConstantBufferViews FogConstantBufferViews;		

	std::shared_ptr<class FRenderingTextureResourcesUpdate> RenderingTexture2DResources;
	std::shared_ptr<class FRenderingTextureResourcesUpdate> RenderingCubeMapResources;
	std::vector<CMaterial*> Materials;
	std::vector<CMeshComponent*> DynamicReflectionMeshComponents;

	CFogComponent* Fog;

	FDynamicShadowMap DynamicShadowMap;

	FDynamicShadowCubeMap DynamicShadowCubeMap;
};
