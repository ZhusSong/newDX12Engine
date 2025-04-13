#pragma once

#if defined(_WIN32)
#include "../Frame/Engine.h"

//Windows���������涨��
class FWindowsEngine :public FEngine
{
public:
	FWindowsEngine();
	~FWindowsEngine();

	virtual int PreInit(FWinMainCommandParameters InParameters);

	virtual int Init(FWinMainCommandParameters InParameters);
	virtual int PostInit();

	virtual void Tick(float DeltaTime);

	virtual int PreExit();
	virtual int Exit();
	virtual int PostExit();

public:
	ID3D12Resource* GetCurrentSwapBuff() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentSwapBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilView() const;

protected:
	void WaitGPUCommandQueueComplete();

private:
	bool InitWindows(FWinMainCommandParameters InParameters);

	bool InitDirect3D();
protected:
	UINT64 CurrentFenceIndex;
	int CurrentSwapBuffIndex;

	//���� DirectX ͼ�λ����ṹ (DXGI) ����
	ComPtr<IDXGIFactory4> DXGIFactory;

	//��������������������б�������С�Fence����Դ���ܵ�״̬���󡢶ѡ���ǩ�����������������Դ��ͼ
	ComPtr<ID3D12Device> D3dDevice;

	//Χ��������ͬ�� CPU �� GPU ����
	ComPtr<ID3D12Fence> Fence;

	//�������
	ComPtr<ID3D12CommandQueue> CommandQueue;

	//����洢
	ComPtr<ID3D12CommandAllocator> CommandAllocator; 

	//�����б�
	ComPtr<ID3D12GraphicsCommandList> GraphicsCommandList;

	ComPtr<IDXGISwapChain> SwapChain;

	//����������Ͷ�
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;

	//�����������ģ��
	vector<ComPtr<ID3D12Resource>> SwapChainBuffer;
	ComPtr<ID3D12Resource> DepthStencilBuffer;

	//��Ļ�ӿ�
	D3D12_VIEWPORT ViewprotInfo;
	D3D12_RECT ViewprotRect;
protected:
	HWND MainWindowsHandle;//��windows���
	UINT M4XQualityLevels;
	bool bMSAA4XEnabled;
	DXGI_FORMAT BackBufferFormat;
	DXGI_FORMAT DepthStencilFormat;
	UINT RTVDescriptorSize;
};
#endif