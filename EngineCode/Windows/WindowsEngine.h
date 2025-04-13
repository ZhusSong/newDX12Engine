#pragma once

#if defined(_WIN32)
#include "../Frame/Engine.h"

//Windows环境的引擎定义
class FWindowsEngine :public FEngine
{
public:
	FWindowsEngine();
	~FWindowsEngine();

	virtual int PreInit(FWinMainCommandParameters InParameters);

	virtual int Init(FWinMainCommandParameters InParameters);
	virtual int PostInit();

	virtual void Tick(float DeltaTime);

	virtual int PreExit();
	virtual int Exit();
	virtual int PostExit();

public:
	ID3D12Resource* GetCurrentSwapBuff() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentSwapBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilView() const;

protected:
	void WaitGPUCommandQueueComplete();

private:
	bool InitWindows(FWinMainCommandParameters InParameters);

	bool InitDirect3D();
protected:
	UINT64 CurrentFenceIndex;
	int CurrentSwapBuffIndex;

	//创建 DirectX 图形基础结构 (DXGI) 对象
	ComPtr<IDXGIFactory4> DXGIFactory;

	//创建命令分配器、命令列表、命令队列、Fence、资源、管道状态对象、堆、根签名、采样器和许多资源视图
	ComPtr<ID3D12Device> D3dDevice;

	//围栏，用于同步 CPU 和 GPU 对象。
	ComPtr<ID3D12Fence> Fence;

	//命令队列
	ComPtr<ID3D12CommandQueue> CommandQueue;

	//命令存储
	ComPtr<ID3D12CommandAllocator> CommandAllocator; 

	//命令列表
	ComPtr<ID3D12GraphicsCommandList> GraphicsCommandList;

	ComPtr<IDXGISwapChain> SwapChain;

	//描述符对象和堆
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;

	//交换链与深度模板
	vector<ComPtr<ID3D12Resource>> SwapChainBuffer;
	ComPtr<ID3D12Resource> DepthStencilBuffer;

	//屏幕视口
	D3D12_VIEWPORT ViewprotInfo;
	D3D12_RECT ViewprotRect;
protected:
	HWND MainWindowsHandle;//主windows句柄
	UINT M4XQualityLevels;
	bool bMSAA4XEnabled;
	DXGI_FORMAT BackBufferFormat;
	DXGI_FORMAT DepthStencilFormat;
	UINT RTVDescriptorSize;
};
#endif