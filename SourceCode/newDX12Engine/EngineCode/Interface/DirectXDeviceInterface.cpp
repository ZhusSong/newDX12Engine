#include "DirectXDeviceInterface.h"
#include "../Windows/WindowsEngine.h"
#include "../Rendering/Engine/DirectX/Core/DirectXRenderingEngine.h"
#include "../Rendering/Core/DirectX/RenderingPipeline/RenderingPipeline.h"
#include "../Mesh/Core/MeshManager.h"
#include "../Rendering/Core/DirectX/RenderingPipeline/RenderLayer/RenderLayerManager.h"
#include "../Core/World.h"
#include "../Core/Camera.h"

void IDirectXDeviceInterface::StartSetMainViewportRenderTarget()
{
	GetEngine()->GetRenderingEngine()->StartSetMainViewportRenderTarget();
}

void IDirectXDeviceInterface::EndSetMainViewportRenderTarget()
{
	GetEngine()->GetRenderingEngine()->EndSetMainViewportRenderTarget();
}

void IDirectXDeviceInterface::ClearMainSwapChainCanvas()
{
	GetEngine()->GetRenderingEngine()->ClearMainSwapChainCanvas();
}

// 得到当前围栏值
ComPtr<ID3D12Fence> IDirectXDeviceInterface::GetFence() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->Fence;
		}
	}

	return NULL;
}

// 得到d3d设备
ComPtr<ID3D12Device> IDirectXDeviceInterface::GetD3dDevice() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->D3dDevice;
		}
	}

	return NULL;
}

CLightManager* IDirectXDeviceInterface::GetLightManager() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->GetLightManager();
		}
	}

	return NULL;
}

CMeshManager* IDirectXDeviceInterface::GetMeshManager() const
{
	return GetEngine()->GetMeshManager();
}


CWorld* IDirectXDeviceInterface::GetWorld() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		return InEngine->GetWorld();
	}

	return NULL;
}

GCamera* IDirectXDeviceInterface::GetCamera() const
{
	if (GetWorld())
	{
		return GetWorld()->GetCamera();
	}

	return nullptr;
}

// 得到图形命令列表
ComPtr<ID3D12GraphicsCommandList> IDirectXDeviceInterface::GetGraphicsCommandList() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->GraphicsCommandList;
		}
	}

	return NULL;
}

// 得到命令分配器
ComPtr<ID3D12CommandAllocator> IDirectXDeviceInterface::GetCommandAllocator() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->CommandAllocator;
		}
	}

	return NULL;
}

// 得到命令队列
ComPtr<ID3D12CommandQueue> IDirectXDeviceInterface::GetCommandQueue() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->CommandQueue;
		}
	}

	return NULL;
}


ID3D12DescriptorHeap* IDirectXDeviceInterface::GetRTVHeap() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->RTVHeap.Get();
		}
	}

	return NULL;
}

ID3D12DescriptorHeap* IDirectXDeviceInterface::GetDSVHeap() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->DSVHeap.Get();
		}
	}

	return NULL;
}

int IDirectXDeviceInterface::GetViewportWidth() const
{
	if (GCamera* InCamera = GetCamera())
	{
		return InCamera->GetWidth();
	}

	return 0;
}

int IDirectXDeviceInterface::GetViewportHeight() const
{
	if (GCamera* InCamera = GetCamera())
	{
		return InCamera->GetHeight();
	}

	return 0;
}

UINT IDirectXDeviceInterface::GetDescriptorHandleIncrementSizeByDSV() const
{
	return GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

UINT IDirectXDeviceInterface::GetDescriptorHandleIncrementSizeByRTV() const
{
	return GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

UINT IDirectXDeviceInterface::GetDescriptorHandleIncrementSizeByCBV_SRV_UAV() const
{
	return GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// 得到围栏序列
UINT64 IDirectXDeviceInterface::GetCurrentFenceIndex() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->CurrentFenceIndex;
		}
	}

	return NULL;
}

// 得到窗口句柄
HWND IDirectXDeviceInterface::GetMainWindowsHandle() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->MainWindowsHandle;
		}
	}

	return HWND();
}

ID3D12Resource* IDirectXDeviceInterface::GetDepthStencilBuffer()
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		if (InEngine->GetRenderingEngine())
		{
			return InEngine->GetRenderingEngine()->DepthStencilBuffer.Get();
		}
	}

	return NULL;
}

FRenderingPipeline* IDirectXDeviceInterface::GetRenderingPipeline() const
{
	if (CMeshManager* MeshManager = GetMeshManager())
	{
		return MeshManager->GetRenderingPipeline();
	}

	return NULL;
}

FRenderLayerManager* IDirectXDeviceInterface::GetRenderLayerManager() const
{
	if (FRenderingPipeline* InRenderingPipeline = GetRenderingPipeline())
	{
		return InRenderingPipeline->GetRenderLayer();
	}

	return nullptr;
}


#if defined(_WIN32)
CWindowsEngine* IDirectXDeviceInterface::GetEngine() const
{
	return dynamic_cast<CWindowsEngine*>(Engine);
}
#else
CEngine* IDirectXDeviceInterface::GetEngine()
{
	return Engine;
}
#endif


ComPtr<ID3D12Fence> IDirectXDeviceInterface_Struct::GetFence() const
{
	return Interface.GetFence();
}

ComPtr<ID3D12Device> IDirectXDeviceInterface_Struct::GetD3dDevice()const
{
	return Interface.GetD3dDevice();
}

CLightManager* IDirectXDeviceInterface_Struct::GetLightManager()const
{
	return Interface.GetLightManager();
}

CMeshManager* IDirectXDeviceInterface_Struct::GetMeshManager()const
{
	return Interface.GetMeshManager();
}

CWorld* IDirectXDeviceInterface_Struct::GetWorld()const
{
	return Interface.GetWorld();
}

ComPtr<ID3D12GraphicsCommandList> IDirectXDeviceInterface_Struct::GetGraphicsCommandList()const
{
	return Interface.GetGraphicsCommandList();
}

ComPtr<ID3D12CommandAllocator> IDirectXDeviceInterface_Struct::GetCommandAllocator()const
{
	return Interface.GetCommandAllocator();
}

ComPtr<ID3D12CommandQueue> IDirectXDeviceInterface_Struct::GetCommandQueue()const
{
	return Interface.GetCommandQueue();
}

ID3D12DescriptorHeap* IDirectXDeviceInterface_Struct::GetRTVHeap()const
{
	return Interface.GetRTVHeap();
}

ID3D12DescriptorHeap* IDirectXDeviceInterface_Struct::GetDSVHeap()const
{
	return Interface.GetDSVHeap();
}

UINT IDirectXDeviceInterface_Struct::GetDescriptorHandleIncrementSizeByDSV()const
{
	return Interface.GetDescriptorHandleIncrementSizeByDSV();
}

UINT IDirectXDeviceInterface_Struct::GetDescriptorHandleIncrementSizeByRTV()const
{
	return Interface.GetDescriptorHandleIncrementSizeByRTV();
}

UINT IDirectXDeviceInterface_Struct::GetDescriptorHandleIncrementSizeByCBV_SRV_UAV()const
{
	return Interface.GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();
}

UINT64 IDirectXDeviceInterface_Struct::GetCurrentFenceIndex()const
{
	return Interface.GetCurrentFenceIndex();
}

HWND IDirectXDeviceInterface_Struct::GetMainWindowsHandle()const
{
	return Interface.GetMainWindowsHandle();
}

ID3D12Resource* IDirectXDeviceInterface_Struct::GetDepthStencilBuffer()
{
	return Interface.GetDepthStencilBuffer();
}

FRenderingPipeline* IDirectXDeviceInterface_Struct::GetRenderingPipeline() const
{
	return Interface.GetRenderingPipeline();
}

FRenderLayerManager* IDirectXDeviceInterface_Struct::GetRenderLayerManager() const
{
	return Interface.GetRenderLayerManager();
}

#if defined(_WIN32)
CWindowsEngine* IDirectXDeviceInterface_Struct::GetEngine()const
{
	return Interface.GetEngine();
}
#else
CEngine* IDirectXDeviceInterface_Struct::GetEngine()const
{
	return Interface.GetEngine();
}

#endif

#if EDITOR_ENGINE
#include "../../EditorEngine/EditorEngine.h"
CEditorEngine* IDirectXDeviceInterface::GetEditorEngine() const
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		return InEngine->EditorEngine;
	}

	return NULL;
}

CEditorEngine* IDirectXDeviceInterface_Struct::GetEditorEngine() const
{
	return Interface.GetEditorEngine();
}
#endif