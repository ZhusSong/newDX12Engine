#pragma once
#include "../../Core/RenderingEngine.h"
#include "../../../../Core/Viewport/ViewportInfo.h"

class CLightManager;
class CMeshManager;
class CWorld;

// DirectX的渲染引擎，继承自渲染引擎基类
// DirectXレンダリングエンジン、レンダリングエンジンベースクラスから継承
class CDirectXRenderingEngine :public CRenderingEngine
{
	// 设置DirectX渲染接口类为友元，使其能够访问protected对象
	// DirectXレンダリングインターフェースクラスをフレンドに設定し、protectedオブジェクトにアクセスできるようにする
	friend class IDirectXDeviceInterface;
	friend class CWindowsEngine;
public:
	CDirectXRenderingEngine();
	~CDirectXRenderingEngine();

	// 引擎流程:预初始化->初始化->后初始化->每帧更新->清理
	// エンジンフロー: プリ初期化 -> 初期化 -> ポスト初期化 -> 毎フレーム更新 -> クリーンアップ
	virtual int PreInit(FWinMainCommandParameters InParameters);

	virtual int Init(FWinMainCommandParameters InParameters);
	virtual int PostInit();

	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	virtual void Tick(float DeltaTime);

	virtual void OnResetSize(int InWidth, int InHeight);

	virtual int PreExit();
	virtual int Exit();
	virtual int PostExit();
public:
	void StartSetMainViewportRenderTarget();
	void EndSetMainViewportRenderTarget();
	void ClearMainSwapChainCanvas();

public:
	// 提供对当前帧缓冲区和深度模板视图的访问
	// 現在のフレームバッファと深度ステンシルビューへのアクセスを提供
	ID3D12Resource* GetCurrentSwapBuff() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentSwapBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilView() const;

	// 检测当前gpu是nvidia还是amd
	// 現在のGPUがNVIDIAかAMDかを検出
	CurrentGPU GetCurrentGPU();
public:
	// 提供当前渲染目标的格式、抗锯齿（MSAA）参数
	// 現在のレンダーターゲットの形式、アンチエイリアシング（MSAA）パラメータを提供
	DXGI_FORMAT GetBackBufferFormat() const { return BackBufferFormat; }
	DXGI_FORMAT GetDepthStencilFormat() const { return DepthStencilFormat; }
	UINT GetDXGISampleCount()const;
	UINT GetDXGISampleQuality()const;

	CMeshManager* GetMeshManager()  const { return MeshManager; }
	CLightManager* GetLightManager()  const { return LightManager; }


private:

protected:

	// 封装D3D初始化与GPU/CPU同步
	// D3D初期化とGPU/CPU同期をカプセル化
	void WaitGPUCommandQueueComplete();

	bool InitDirect3D();
	void PostInitDirect3D();

protected:
	// Manager管理
	// マネージャー管理
	CLightManager* LightManager;
	CMeshManager* MeshManager;
	CWorld* World;
protected:
	// 帧同步与交换链缓冲区索引
	// フレーム同期とスワップチェーンバッファインデックス
	UINT64 CurrentFenceIndex;
	int CurrentSwapBuffIndex;

	//创建 DirectX 图形基础结构 (DXGI) 对象
	// DirectX グラフィックス基盤 (DXGI) オブジェクトを作成
	ComPtr<IDXGIFactory4> DXGIFactory;
	// 创建命令分配器、命令列表、命令队列、Fence、资源、管道状态对象、堆、根签名、采样器和许多资源视图
	// コマンドアロケータ、コマンドリスト、コマンドキュー、フェンス、リソース、パイプラインステートオブジェクト、ヒープ、ルートシグネチャ、サンプラー、および多数のリソースビューを作成
	ComPtr<ID3D12Device> D3dDevice;
	// 一个用于同步 CPU 和一个或多个 GPU 的对象。
	// CPUと1つ以上のGPUを同期するためのオブジェクト
	ComPtr<ID3D12Fence> Fence;

	// 队列
	//  キュー
	ComPtr<ID3D12CommandQueue> CommandQueue;
	// 存储 
	// ストレージ
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	// 命令列表
	// コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> GraphicsCommandList;

	// 交换链
	// スワップチェーン
	ComPtr<IDXGISwapChain> SwapChain;

	// 描述符对象和堆
	// 記述子オブジェクトとヒープ
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;

	vector<ComPtr<ID3D12Resource>> SwapChainBuffer;
	ComPtr<ID3D12Resource> DepthStencilBuffer;

	
protected:
	// MSAA设置与RTV句柄大小
	// MSAA設定とRTVハンドルサイズ
	UINT M4XQualityLevels;
	bool bMSAA4XEnabled;
	DXGI_FORMAT BackBufferFormat;
	DXGI_FORMAT DepthStencilFormat;
	UINT RTVDescriptorSize;
};