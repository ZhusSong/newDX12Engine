
#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"
#include "Core/RenderLayer.h"

class GActorObject;
class CComponent;
struct FRenderingData;
class FRenderLayerManager :public IDirectXDeviceInterface
{
	friend class FRenderLayer;
	friend struct FGeometry;
	friend struct FGeometryMap;
public:
	FRenderLayerManager();
	~FRenderLayerManager();

	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState);
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	virtual void OnResetSize(int InWidth, int InHeight);
	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);


	virtual void Draw(int InLayer, float DeltaTime);
	virtual void FindObjectDraw(float DeltaTime, int InLayer, const CMeshComponent* InKey);


	virtual void BuildPSO();

	virtual void HighlightDisplayObject(GActorObject* InObject);
	virtual void HighlightDisplayObject(std::weak_ptr<FRenderingData> RenderingData);
	virtual void HighlightDisplayObject(CComponent* RenderingData);


	// 排序
	// ソート
	virtual void Sort();

	// 单独设置PSO
	// PSOを個別に設定する
	virtual void ResetPSO(int InLayer);

	// 通过流水线状态设置PSO
	// パイプラインステートを通じてPSOを設定する
	virtual void ResetPSO(int InLayer, EPipelineState InPipelineState);

	// 渲染 不包含设置PSO
	// PSO設定を含まない描画
	virtual void DrawMesh(float DeltaTime, int InLayer, ERenderingConditions RC = ERenderingConditions::RC_None);

	static std::shared_ptr<FRenderLayer> FindByRenderLayer(int InRenderLayer);

public:
	void Add(int InLayer, std::weak_ptr<FRenderingData> InRenderingData);
	void Remove(int InLayer, std::weak_ptr<FRenderingData> InRenderingData);

	void Clear(int InLayer);

protected:
	static std::vector<std::shared_ptr<FRenderLayer>> RenderLayers;
};

// 注册渲染层级
// 描画レイヤーを登録する
template<class T>
std::shared_ptr<T> CreateRenderLayer()
{
	std::shared_ptr<T> RenderLayer = std::make_shared<T>();
	RenderLayer->RegisterRenderLayer();

	return RenderLayer;
}