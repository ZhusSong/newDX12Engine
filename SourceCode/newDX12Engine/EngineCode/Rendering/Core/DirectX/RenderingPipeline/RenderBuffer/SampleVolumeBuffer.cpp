#include "SampleVolumeBuffer.h"

FSampleVolumeBuffer::FSampleVolumeBuffer()
{
	BuildRandomVolumeBuffer();
}

void FSampleVolumeBuffer::BuildVolumeBuffer()
{
	VolumeBuffer.clear();

	// 构造采样需要的八个顶点
	VolumeBuffer.push_back(fvector_4d(+1.0f, +1.0f, +1.0f, 0.0f));
	VolumeBuffer.push_back(fvector_4d(-1.0f, -1.0f, -1.0f, 0.0f));

	VolumeBuffer.push_back(fvector_4d(-1.0f, +1.0f, +1.0f, 0.0f));
	VolumeBuffer.push_back(fvector_4d(+1.0f, -1.0f, -1.0f, 0.0f));

	VolumeBuffer.push_back(fvector_4d(+1.0f, +1.0f, -1.0f, 0.0f));
	VolumeBuffer.push_back(fvector_4d(-1.0f, -1.0f, +1.0f, 0.0f));

	VolumeBuffer.push_back(fvector_4d(-1.0f, +1.0f, -1.0f, 0.0f));
	VolumeBuffer.push_back(fvector_4d(+1.0f, -1.0f, +1.0f, 0.0f));

	// 
	VolumeBuffer.push_back(fvector_4d(-1.0f, 0.0f, 0.0f, 0.0f));
	VolumeBuffer.push_back(fvector_4d(+1.0f, 0.0f, 0.0f, 0.0f));

	VolumeBuffer.push_back(fvector_4d(0.0f, -1.0f, 0.0f, 0.0f));
	VolumeBuffer.push_back(fvector_4d(0.0f, +1.0f, 0.0f, 0.0f));

	VolumeBuffer.push_back(fvector_4d(0.0f, 0.0f, -1.0f, 0.0f));
	VolumeBuffer.push_back(fvector_4d(0.0f, 0.0f, +1.0f, 0.0f));
}

// 创建噪声缓冲区
void FSampleVolumeBuffer::BuildRandomVolumeBuffer()
{
	if (VolumeBuffer.size() == 0)
	{
		BuildVolumeBuffer();
	}

	for (auto& Tmp : VolumeBuffer)
	{
		Tmp.normalize();

		// 获取随机数
		Tmp *= get_float_random_in_range(0.24f, 1.f);
	}
}

void FSampleVolumeBuffer::Update(XMFLOAT4* InBuffer)
{
	memcpy(InBuffer, GetVolumeBuffer(), VolumeBuffer.size() * sizeof(fvector_4d));
}
