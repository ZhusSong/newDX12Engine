#include "RenderingResourcesUpdate.h"

FRenderingResourcesUpdate::FRenderingResourcesUpdate()
	: ElementSize(0)
	, ElementDataSize(0)
	, Data(nullptr)
{

}

FRenderingResourcesUpdate::~FRenderingResourcesUpdate()
{
	if (UploadBuffer != nullptr)
	{
		UploadBuffer->Unmap(0, NULL);
		UploadBuffer = nullptr;
	}
}
/// <summary>
/// 初始化对象所对应的常量缓冲区
/// オブジェクトに対応する定数バッファを初期化する
/// </summary>
/// <param name="InDevice">当前D3D设备</param>
/// <param name="InElemetSize">对象内存空间</param>
/// <param name="InElemetCount">对象数</param>
/// 
/// <param name="InDevice">現在のD3Dデバイス</param>
/// <param name="InElemetSize">オブジェクトのメモリサイズ</param>
/// <param name="InElemetCount">オブジェクトの数</param>
void FRenderingResourcesUpdate::Init(ID3D12Device* InDevice, UINT InElemetSize, UINT InElemetCount, bool bConstBuffer)
{
	assert(InDevice);

	ElementDataSize = InElemetSize;

	if (bConstBuffer)
	{
		ElementSize = GetConstantBufferByteSize(InElemetSize);
	}
	else
	{
		ElementSize = InElemetSize;
	}


	CD3DX12_HEAP_PROPERTIES HeapPropertie = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(ElementSize * InElemetCount);
	ANALYSIS_HRESULT(InDevice->CreateCommittedResource(
		&HeapPropertie,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&UploadBuffer)));

	ANALYSIS_HRESULT(UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&Data)));
}


void FRenderingResourcesUpdate::Update(int Index, const void* InData)
{
	// 常量缓冲区按 256 字节对齐时，先清零整块再复制真实数据大小，
	// 以避免结构体比对齐尺寸更小时发生越界读取。
	// 定数バッファが 256 バイト境界に揃えられる場合は、
	// 先にブロック全体を 0 で埋めてから実データサイズだけをコピーし、
	// 構造体サイズが整列サイズより小さいときの範囲外読み取りを防ぐ。
	BYTE* Dest = &Data[Index * ElementSize];
	memset(Dest, 0, ElementSize);
	memcpy(Dest, InData, ElementDataSize);
}

UINT FRenderingResourcesUpdate::GetConstantBufferByteSize(UINT InTypeSzie)
{

	return (InTypeSzie + 255) & ~255;
}

UINT FRenderingResourcesUpdate::GetConstantBufferByteSize()
{
	return GetConstantBufferByteSize(ElementSize);
}
