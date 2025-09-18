#pragma once
// 采样缓冲区
#include "Core/RenderBuffer.h"

struct FSampleVolumeBuffer
{
	FSampleVolumeBuffer();

public:
	void BuildVolumeBuffer();
	void BuildRandomVolumeBuffer();

	void Update(XMFLOAT4* InBuffer);
public:
	void* GetVolumeBuffer() { return VolumeBuffer.data(); }

protected:
	std::vector<fvector_4d> VolumeBuffer;
};