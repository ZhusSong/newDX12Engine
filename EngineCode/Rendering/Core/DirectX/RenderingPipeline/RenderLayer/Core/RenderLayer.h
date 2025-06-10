#pragma once

//25.6.10 李
#include "../../../../../../Interface/DirectXDeviceInterface.h"
#include "../../../../../../Shader/Core/Shader.h"
#include "../../Geometry/RenderingData.h"

struct FDirectXPipelineState;
struct FGeometryMap;
struct FRenderingData;
struct FViewportInfo;

class FRenderLayer 
	:public IDirectXDeviceInterface
	,public std::enable_shared_from_this<FRenderLayer>
{
	friend struct FGeometry;
	friend struct FGeometryMap;
public:
	FRenderLayer();

	//基础注册环节
	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState);

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);

	void RegisterRenderLayer();

	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);
public:
	const UINT GetRenderPriority()const { return RenderPriority; }

public:
	virtual void BuildShader() = 0;
	virtual int GetRenderLayerType()const = 0;
protected:
	UINT RenderPriority;

protected:
	FShader VertexShader;
	FShader PixelShader;
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;

	std::vector<FRenderingData> RenderDatas;

	FGeometryMap* GeometryMap;
	FDirectXPipelineState* DirectXPipelineState;
};