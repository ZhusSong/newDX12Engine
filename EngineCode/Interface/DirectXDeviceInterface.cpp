#include "DirectXDeviceInterface.h"
#include "../Windows/WindowsEngine.h"
#include "../Rendering/Engine/DirectX/Core/DirectXRenderingEngine.h"

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
ComPtr<ID3D12Fence> IDirectXDeviceInterface::GetFence()
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
ComPtr<ID3D12Device> IDirectXDeviceInterface::GetD3dDevice()
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

CLightManager* IDirectXDeviceInterface::GetLightManager()
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

CMeshManager* IDirectXDeviceInterface::GetMeshManager()
{
	return GetEngine()->GetMeshManager();
}


CWorld* IDirectXDeviceInterface::GetWorld()
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		return InEngine->GetWorld();
	}

	return NULL;
}

// 得到图形命令列表
ComPtr<ID3D12GraphicsCommandList> IDirectXDeviceInterface::GetGraphicsCommandList()
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
ComPtr<ID3D12CommandAllocator> IDirectXDeviceInterface::GetCommandAllocator()
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
ComPtr<ID3D12CommandQueue> IDirectXDeviceInterface::GetCommandQueue()
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


ID3D12DescriptorHeap* IDirectXDeviceInterface::GetRTVHeap()
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

ID3D12DescriptorHeap* IDirectXDeviceInterface::GetDSVHeap()
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

UINT IDirectXDeviceInterface::GetDescriptorHandleIncrementSizeByDSV()
{
	return GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

UINT IDirectXDeviceInterface::GetDescriptorHandleIncrementSizeByRTV()
{
	return GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

UINT IDirectXDeviceInterface::GetDescriptorHandleIncrementSizeByCBV_SRV_UAV()
{
	return GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// 得到围栏序列
UINT64 IDirectXDeviceInterface::GetCurrentFenceIndex()
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
HWND IDirectXDeviceInterface::GetMainWindowsHandle()
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


#if defined(_WIN32)
CWindowsEngine* IDirectXDeviceInterface::GetEngine()
{
	return dynamic_cast<CWindowsEngine*>(Engine);
}
#else
CEngine* IDirectXDeviceInterfece::GetEngine()
{
	return Engine;
}
#endif


ComPtr<ID3D12Fence> IDirectXDeviceInterface_Struct::GetFence()
{
	return Interface.GetFence();
}

ComPtr<ID3D12Device> IDirectXDeviceInterface_Struct::GetD3dDevice()
{
	return Interface.GetD3dDevice();
}

CLightManager* IDirectXDeviceInterface_Struct::GetLightManager()
{
	return Interface.GetLightManager();
}

CMeshManager* IDirectXDeviceInterface_Struct::GetMeshManager()
{
	return Interface.GetMeshManager();
}

CWorld* IDirectXDeviceInterface_Struct::GetWorld()
{
	return Interface.GetWorld();
}

ComPtr<ID3D12GraphicsCommandList> IDirectXDeviceInterface_Struct::GetGraphicsCommandList()
{
	return Interface.GetGraphicsCommandList();
}

ComPtr<ID3D12CommandAllocator> IDirectXDeviceInterface_Struct::GetCommandAllocator()
{
	return Interface.GetCommandAllocator();
}

ComPtr<ID3D12CommandQueue> IDirectXDeviceInterface_Struct::GetCommandQueue()
{
	return Interface.GetCommandQueue();
}

ID3D12DescriptorHeap* IDirectXDeviceInterface_Struct::GetRTVHeap()
{
	return Interface.GetRTVHeap();
}

ID3D12DescriptorHeap* IDirectXDeviceInterface_Struct::GetDSVHeap()
{
	return Interface.GetDSVHeap();
}

UINT IDirectXDeviceInterface_Struct::GetDescriptorHandleIncrementSizeByDSV()
{
	return Interface.GetDescriptorHandleIncrementSizeByDSV();
}

UINT IDirectXDeviceInterface_Struct::GetDescriptorHandleIncrementSizeByRTV()
{
	return Interface.GetDescriptorHandleIncrementSizeByRTV();
}

UINT IDirectXDeviceInterface_Struct::GetDescriptorHandleIncrementSizeByCBV_SRV_UAV()
{
	return Interface.GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();
}

UINT64 IDirectXDeviceInterface_Struct::GetCurrentFenceIndex()
{
	return Interface.GetCurrentFenceIndex();
}

HWND IDirectXDeviceInterface_Struct::GetMianWindowsHandle()
{
	return Interface.GetMainWindowsHandle();
}

#if defined(_WIN32)
CWindowsEngine* IDirectXDeviceInterface_Struct::GetEngine()
{
	return Interface.GetEngine();
}
#else
CEngine* IDirectXDeviceInterfece_Struct::GetEngine()
{
	return Interfece.GetEngine();
}
#endif


#if EDITOR_ENGINE
#include "../../EditorEngine/EditorEngine.h"
CEditorEngine* IDirectXDeviceInterface::GetEditorEngine()
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		return InEngine->EditorEngine;
	}

	return NULL;
}

CEditorEngine* IDirectXDeviceInterface_Struct::GetEditorEngine()
{
	return Interface.GetEditorEngine();
}
#endif