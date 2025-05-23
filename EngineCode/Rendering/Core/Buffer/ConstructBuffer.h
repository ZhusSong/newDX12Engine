﻿// 2025.4.李
#pragma once
#include "../../../Core/Engine.h"
#include "../../../Interface/DirectXDeviceInterface.h"

namespace ConstructBuffer
{
	struct FConstructBuffer :public IDirectXDeviceInterface_Struct
	{
		ComPtr<ID3D12Resource> ConstructDefaultBuffer(ComPtr<ID3D12Resource>& OutTmpBuffer, const void* InData, UINT64 InDataSize);
	};
}