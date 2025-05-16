#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"

// 描述符结构体
struct FDirectXDescriptorHeap :public IDirectXDeviceInterface_Struct
{
	void Build(UINT InNumDescriptor);

	void PreDraw(float DeltaTime);

	ID3D12DescriptorHeap* GetHeap()const { return CBVHeap.Get(); }
protected:
	ComPtr<ID3D12DescriptorHeap> CBVHeap;
};