// 25.7.7 李
#pragma once
#include "../Core/Engine.h"

#if defined(_WIN32)
class CWindowsEngine;
#else
class CEngine;
#endif

class CLightManager;
class CMeshManager;
class CWorld;
class FRenderLayerManager;

struct FRenderingPipeline;

// 提供渲染设备内容的接口
class IDirectXDeviceInterface
{
	friend struct IDirectXDeviceInterface_Struct;
public:
	// 得到视口RenderTarget
	void StartSetMainViewportRenderTarget();
	void EndSetMainViewportRenderTarget();
	void ClearMainSwapChainCanvas();

public:
	ComPtr<ID3D12Fence> GetFence()const;
	ComPtr<ID3D12Device> GetD3dDevice()const;
	CLightManager* GetLightManager()const;
	CMeshManager* GetMeshManager()const;
	CWorld* GetWorld()const;

	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList()const;
	ComPtr<ID3D12CommandAllocator> GetCommandAllocator()const;
	ComPtr<ID3D12CommandQueue> GetCommandQueue()const;

	ID3D12DescriptorHeap* GetRTVHeap()const;
	ID3D12DescriptorHeap* GetDSVHeap()const;

	UINT GetDescriptorHandleIncrementSizeByDSV()const;
	UINT GetDescriptorHandleIncrementSizeByRTV()const;
	UINT GetDescriptorHandleIncrementSizeByCBV_SRV_UAV()const;

	UINT64 GetCurrentFenceIndex()const;
	HWND GetMainWindowsHandle()const;

	ID3D12Resource* GetDepthStencilBuffer();

protected:
	FRenderingPipeline* GetRenderingPipeline() const;
	FRenderLayerManager* GetRenderLayerManager() const;

public:
#if defined(_WIN32)
	CWindowsEngine* GetEngine() const;
#else
	CEngine* GetEngine();
#endif

	// 添加编辑器引擎
#if EDITOR_ENGINE
	class CEditorEngine* GetEditorEngine() const;
#endif // 0
};

// 提供渲染设备接口的结构体
struct IDirectXDeviceInterface_Struct
{
public:
	ComPtr<ID3D12Fence> GetFence() const;
	ComPtr<ID3D12Device> GetD3dDevice() const;

	CLightManager* GetLightManager() const;
	CMeshManager* GetMeshManager() const;
	CWorld* GetWorld() const;

	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList() const;
	ComPtr<ID3D12CommandAllocator> GetCommandAllocator() const;
	ComPtr<ID3D12CommandQueue> GetCommandQueue() const;

	ID3D12DescriptorHeap* GetRTVHeap() const;

	ID3D12DescriptorHeap* GetDSVHeap() const;

	UINT GetDescriptorHandleIncrementSizeByDSV() const;

	UINT GetDescriptorHandleIncrementSizeByRTV() const;

	UINT GetDescriptorHandleIncrementSizeByCBV_SRV_UAV() const;

	UINT64 GetCurrentFenceIndex() const;
	HWND GetMainWindowsHandle() const;

	ID3D12Resource* GetDepthStencilBuffer();

protected:
	FRenderingPipeline* GetRenderingPipeline() const;
	FRenderLayerManager* GetRenderLayerManager() const;
#if defined(_WIN32)
	CWindowsEngine* GetEngine() const;
#else
	CEngine* GetEngine() const;
#endif

#if EDITOR_ENGINE
	class CEditorEngine* GetEditorEngine() const;
#endif // 0

private:
	// 创建接口的实例
	IDirectXDeviceInterface Interface;
};