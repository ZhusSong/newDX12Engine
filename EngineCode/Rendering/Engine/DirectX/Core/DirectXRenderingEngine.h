#pragma once
#include "../../Core/RenderingEngine.h"

class CMeshManage;
// DirectX的渲染引擎，继承自渲染引擎基类
class CDirectXRenderingEngine :public CRenderingEngine
{
	// 设置DirectX渲染接口类为友元，使其能够访问protected对象
	friend class IDirectXDeviceInterface;
public:
	CDirectXRenderingEngine();
	~CDirectXRenderingEngine();

	// 引擎流程:预初始化->初始化->后初始化->每帧更新->清理
	virtual int PreInit(FWinMainCommandParameters InParameters);

	virtual int Init(FWinMainCommandParameters InParameters);
	virtual int PostInit();

	virtual void Tick(float DeltaTime);

	virtual int PreExit();
	virtual int Exit();
	virtual int PostExit();
public:
	// 提供对当前帧缓冲区和深度模板视图的访问
	ID3D12Resource* GetCurrentSwapBuff() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentSwapBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilView() const;
public:
	// 提供当前渲染目标的格式、抗锯齿（MSAA）参数
	DXGI_FORMAT GetBackBufferFormat() const { return BackBufferFormat; }
	DXGI_FORMAT GetDepthStencilFormat() const { return DepthStencilFormat; }
	UINT GetDXGISampleCount()const;
	UINT GetDXGISampleQuality()const;
protected:
	// 封装D3D初始化与GPU/CPU同步
	void WaitGPUCommandQueueComplete();

	bool InitDirect3D();
	void PostInitDirect3D();

protected:
	// mesh管理
	CMeshManage* MeshManage;
protected:
	// 帧同步与交换链缓冲区索引
	UINT64 CurrentFenceIndex;
	int CurrentSwapBuffIndex;

	//创建 DirectX 图形基础结构 (DXGI) 对象
	ComPtr<IDXGIFactory4> DXGIFactory;
	//创建命令分配器、命令列表、命令队列、Fence、资源、管道状态对象、堆、根签名、采样器和许多资源视图
	ComPtr<ID3D12Device> D3dDevice;
	//一个用于同步 CPU 和一个或多个 GPU 的对象。
	ComPtr<ID3D12Fence> Fence;

	//队列
	ComPtr<ID3D12CommandQueue> CommandQueue;
	//存储
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	//命令列表
	ComPtr<ID3D12GraphicsCommandList> GraphicsCommandList;

	// 交换链
	ComPtr<IDXGISwapChain> SwapChain;

	//描述符对象和堆
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;

	vector<ComPtr<ID3D12Resource>> SwapChainBuffer;
	ComPtr<ID3D12Resource> DepthStencilBuffer;

	//和屏幕的视口有关
	D3D12_VIEWPORT ViewprotInfo;
	D3D12_RECT ViewprotRect;
protected:
	// MSAA设置与RTV句柄大小
	UINT M4XQualityLevels;
	bool bMSAA4XEnabled;
	DXGI_FORMAT BackBufferFormat;
	DXGI_FORMAT DepthStencilFormat;
	UINT RTVDescriptorSize;
};