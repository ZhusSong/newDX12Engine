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
	// 画面の幅と高さに基づいて初期化
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
	// 几何Map
	// ジオメトリマップ
	FGeometryMap* GeometryMap;

	// 管线对象 用于绑定
	// パイプラインオブジェクト（バインド用）
	FDirectXPipelineState* DirectXPipelineState;

	// 渲染层级
	// レンダリングレイヤー
	FRenderLayerManager* RenderLayer;

	UINT Width;
	UINT Height;

	// Shader寄存器
	// シェーダーレジスタ
	UINT SRVOffset;

	// 渲染目标的偏移
	// レンダーターゲットのオフセット
	UINT RTVOffset;

	// 渲染目标
	// レンダーターゲット
	std::shared_ptr<FRenderTarget> RenderTarget;
};

template<class T>
void FDynamicMap::CreateRenderTarget()
{
	if (!RenderTarget)
	{
		RenderTarget = std::make_unique<T>();
	}
}
