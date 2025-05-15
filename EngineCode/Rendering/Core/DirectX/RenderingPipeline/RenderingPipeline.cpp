﻿#include "RenderingPipeline.h"

 // 渲染流水线
FRenderingPipeline::FRenderingPipeline()
{

}
// 创建mesh
void FRenderingPipeline::BuildMesh(GMesh* InMesh, const FMeshRenderingData& MeshData)
{
	GeometryMap.BuildMesh(InMesh, MeshData);
}

void FRenderingPipeline::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	GeometryMap.UpdateCalculations(DeltaTime, ViewportInfo);
}

void FRenderingPipeline::BuildPipeline()
{
	// 先进行重置
	DirectXPipelineState.ResetGPSDesc();

	// 构建根签名
	RootSignature.BuildRootSignature();
	DirectXPipelineState.BindRootSignature(RootSignature.GetRootSignature());

	// 构建Shader
	// HLSL
	VertexShader.BuildShaders(L"../newDX12Engine/Shader/VertexShader.hlsl", "VertexShaderMain", "vs_5_0");
	PixelShader.BuildShaders(L"../newDX12Engine/Shader/VertexShader.hlsl", "PixelShaderMain", "ps_5_0");
	
	// 绑定
	DirectXPipelineState.BindShader(VertexShader, PixelShader);

	// 输入布局
	InputElementDesc =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	// 绑定
	DirectXPipelineState.BindInputLayout(InputElementDesc.data(), InputElementDesc.size());

	// 构建模型
	GeometryMap.Build();

	// 构建常量描述堆
	GeometryMap.BuildDescriptorHeap();

	//构建Mesh常量缓冲区
	GeometryMap.BuildMeshConstantBuffer();

	//构建材质常量缓冲区
	GeometryMap.BuildMaterialConstantBuffer();

	//构建灯光常量缓冲区
	GeometryMap.BuildLightConstantBuffer();

	// 构建视口常量缓冲区视图
	GeometryMap.BuildViewportConstantBufferView();

	// 构建管线
	DirectXPipelineState.Build();
}

void FRenderingPipeline::PreDraw(float DeltaTime)
{
	DirectXPipelineState.PreDraw(DeltaTime);
}

void FRenderingPipeline::Draw(float DeltaTime)
{
	GeometryMap.PreDraw(DeltaTime);
	RootSignature.PreDraw(DeltaTime);

	GeometryMap.Draw(DeltaTime);

	DirectXPipelineState.Draw(DeltaTime);
}

void FRenderingPipeline::PostDraw(float DeltaTime)
{
	GeometryMap.PostDraw(DeltaTime);
	DirectXPipelineState.PostDraw(DeltaTime);
}
