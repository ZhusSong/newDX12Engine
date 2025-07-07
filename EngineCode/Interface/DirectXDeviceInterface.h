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

// 提供渲染设备内容的接口
class IDirectXDeviceInterface
{
public:
	// 得到视口RenderTarget
	void StartSetMainViewportRenderTarget();
	void EndSetMainViewportRenderTarget();
	void ClearMainSwapChainCanvas();

public:
	ComPtr<ID3D12Fence> GetFence();
	ComPtr<ID3D12Device> GetD3dDevice();

	CLightManager* GetLightManager();
	CMeshManager* GetMeshManager();
	CWorld* GetWorld();


	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList();
	ComPtr<ID3D12CommandAllocator> GetCommandAllocator();
	ComPtr<ID3D12CommandQueue> GetCommandQueue();

	ID3D12DescriptorHeap* GetRTVHeap();
	ID3D12DescriptorHeap* GetDSVHeap();

	UINT GetDescriptorHandleIncrementSizeByDSV();
	UINT GetDescriptorHandleIncrementSizeByRTV();
	UINT GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();

	UINT64 GetCurrentFenceIndex();
	HWND GetMainWindowsHandle();

#if defined(_WIN32)
	CWindowsEngine* GetEngine();
#else
	CEngine* GetEngine();
#endif

	// 添加编辑器引擎
#if EDITOR_ENGINE
	class CEditorEngine* GetEditorEngine();
#endif // 0
};

// 提供渲染设备接口的结构体
struct IDirectXDeviceInterface_Struct
{
public:
	ComPtr<ID3D12Fence> GetFence();
	ComPtr<ID3D12Device> GetD3dDevice();

	CLightManager* GetLightManager();
	CMeshManager* GetMeshManager();
	CWorld* GetWorld();

	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList();
	ComPtr<ID3D12CommandAllocator> GetCommandAllocator();
	ComPtr<ID3D12CommandQueue> GetCommandQueue();

	ID3D12DescriptorHeap* GetRTVHeap();

	ID3D12DescriptorHeap* GetDSVHeap();

	UINT GetDescriptorHandleIncrementSizeByDSV();

	UINT GetDescriptorHandleIncrementSizeByRTV();

	UINT GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();

	UINT64 GetCurrentFenceIndex();
	HWND GetMianWindowsHandle();

#if defined(_WIN32)
	CWindowsEngine* GetEngine();
#else
	CEngine* GetEngine();
#endif

#if EDITOR_ENGINE
	class CEditorEngine* GetEditorEngine();
#endif // 0

private:
	// 创建接口的实例
	IDirectXDeviceInterface Interface;
};