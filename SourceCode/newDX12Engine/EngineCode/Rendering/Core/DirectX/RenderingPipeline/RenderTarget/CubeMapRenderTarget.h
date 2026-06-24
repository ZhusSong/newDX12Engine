#pragma once
#include "Core/RenderTarget.h"

// 提供渲染目标的接口
// レンダーターゲットを提供するインターフェース
class FCubeMapRenderTarget :public FRenderTarget
{
	typedef FRenderTarget Super;
	friend class FDynamicCubeMap;
public:
	FCubeMapRenderTarget();

	void Init(UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat);
	void SetOptimizedClearColor(const float InColor[4]);
public:
	// 获取cpu渲染目标视图
	// CPUレンダーターゲットビューを取得する
	CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCPURenderTargetView(int Index);


private:
	virtual void BuildRenderTargetMap();
	virtual void BuildSRVDescriptors();
	virtual void BuildRTVDescriptors();

private:
	//RTV
	std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> CPURenderTargetView;
	float OptimizedClearColor[4];
};
