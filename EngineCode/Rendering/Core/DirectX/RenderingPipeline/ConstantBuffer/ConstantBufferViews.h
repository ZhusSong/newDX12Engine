#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"
#include "../../../RenderingResourcesUpdate.h"

// 常量缓冲区视图结构体
struct FConstantBufferViews :public IDirectXDeviceInterface_Struct
{
	void CreateConstant(UINT ObjectSize, UINT ObjectCount, bool bConstBuffer = true);

	void Update(int Index, const void* InData);

	void BuildConstantBuffer(
		CD3DX12_CPU_DESCRIPTOR_HANDLE InHandle,
		UINT InConstantBufferNum,
		UINT InHandleOffset = 0);


	ID3D12Resource* GetBuffer() { return Constant->GetBuffer(); }
protected:
	shared_ptr<FRenderingResourcesUpdate> Constant;
};
