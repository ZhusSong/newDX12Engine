#pragma once
#include "../../../../Interface/DirectXDeviceInterfece.h"
#include "Geometry/GeometryMap.h"
#include "../../../../Shader/Core/Shader.h"
#include "PipelineState/DirectXPipelineState.h"
#include "RootSignature/DirectXRootSignature.h"
#include "../../../../Core/Viewport/ViewportInfo.h"

// 渲染流水线基类
class GMesh; 
class FRenderingPipeline :public IDirectXDeviceInterface
{
public:
	FRenderingPipeline();

	// 构建Mesh
	void BuildMesh(GMesh* InMesh, const FMeshRenderingData& MeshData);

	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	// 构建渲染流水线
	void BuildPipeline();

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);
protected:

	FShader VertexShader;
	FShader PixelShader;
	vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc;

	FDirectXPipelineState DirectXPipelineState;
	FDirectXRootSignature RootSignature;
	FGeometryMap GeometryMap;
};