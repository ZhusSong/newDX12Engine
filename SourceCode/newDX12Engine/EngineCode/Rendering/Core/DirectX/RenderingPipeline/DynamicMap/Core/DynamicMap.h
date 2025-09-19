//25.6.25 李
#pragma once
#include "../../../../../../Interface/DirectXDeviceInterface.h"
#include "../../RenderTarget/Core/RenderTarget.h"

class FRenderLayerManager;

struct FGeometryMap;
struct FDirectXPipelineState;
struct FViewportInfo;

class FDynamicMap :public IDirectXDeviceInterface
{
public:
	FDynamicMap();

public:
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);
	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	
	// 根据屏幕宽高进行初始化
	virtual void Init(int InWidth, int InHeight);
	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);

	virtual void BuildSRVOffset();
	virtual void BuildRTVOffset();

public:
	template<class T>
	void CreateRenderTarget();


	std::shared_ptr<FRenderTarget> GetRenderTarget() const { return RenderTarget; }

public:

	void SetSRVOffset(UINT InOffset) { SRVOffset = InOffset; }
	UINT GetSRVOffset() { return SRVOffset; }

	void SetRTVOffset(UINT InOffset) { RTVOffset = InOffset; }
	UINT GetRTVOffset() { return RTVOffset; }

	UINT GetWidth()const { return Width; }
	UINT GetHeight()const { return Height; }

protected:
	FGeometryMap* GeometryMap;//几何Map
	FDirectXPipelineState* DirectXPipelineState;//管线对象 用于绑定

	FRenderLayerManager* RenderLayer;//渲染层级

	UINT Width;
	UINT Height;

	UINT SRVOffset;//Shader寄存器
	UINT RTVOffset;//渲染目标的偏移

	std::shared_ptr<FRenderTarget> RenderTarget;//渲染目标
};

template<class T>
void FDynamicMap::CreateRenderTarget()
{
	if (!RenderTarget)
	{
		RenderTarget = std::make_unique<T>();
	}
}
