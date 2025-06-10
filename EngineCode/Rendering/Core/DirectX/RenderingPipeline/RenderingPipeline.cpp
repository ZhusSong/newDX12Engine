#include "RenderingPipeline.h"

FRenderingPipeline::FRenderingPipeline()
{

}

void FRenderingPipeline::BuildMesh(const size_t InMeshHash, CMeshComponent* InMesh, const FMeshRenderingData& MeshData)
{
	GeometryMap.BuildMesh(InMeshHash, InMesh, MeshData);
}

void FRenderingPipeline::DuplicateMesh(CMeshComponent* InMesh, const FRenderingData& MeshData)
{
	GeometryMap.DuplicateMesh(InMesh, MeshData);
}

bool FRenderingPipeline::FindMeshRenderingDataByHash(const size_t& InHash, FRenderingData& MeshData, int InRenderLayerIndex)
{
	return GeometryMap.FindMeshRenderingDataByHash(InHash, MeshData, InRenderLayerIndex);
}



void FRenderingPipeline::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	GeometryMap.UpdateCalculations(DeltaTime, ViewportInfo);
	RenderLayer.UpdateCalculations(DeltaTime, ViewportInfo);
}

void FRenderingPipeline::BuildPipeline()
{
	// 初始化GPS描述
	DirectXPipelineState.ResetGPSDesc();

	// 渲染层级的初始化
	RenderLayer.Init(&GeometryMap, &DirectXPipelineState);

	// 对渲染层级进行排序
	RenderLayer.Sort();

	// 读取贴图纹理
	GeometryMap.LoadTexture();

	// 构建根签名
	RootSignature.BuildRootSignature(GeometryMap.GetDrawTextureResourcesNumber());
	DirectXPipelineState.BindRootSignature(RootSignature.GetRootSignature());

	//渲染每个层级的Shader
	RenderLayer.BuildShader();

	//构建模型
	GeometryMap.Build();

	//构建常量描述堆
	GeometryMap.BuildDescriptorHeap();

	//构建常量缓冲区
	GeometryMap.BuildMeshConstantBuffer();

	//构建材质常量缓冲区
	GeometryMap.BuildMaterialShaderResourceView();

	//构建灯光常量缓冲区
	GeometryMap.BuildLightConstantBuffer();

	//构建我们的视口常量缓冲区视图
	GeometryMap.BuildViewportConstantBufferView();

	//构建贴图
	GeometryMap.BuildTextureConstantBuffer();

	//构建我们的管线
	DirectXPipelineState.Build();

	//// 构建Shader
	//// HLSL
	//char TextureNumBuff[20] = { 0 };
	//D3D_SHADER_MACRO ShaderMacro[] =
	//{
	//	"TEXTURE2D_MAP_NUM",_itoa(GeometryMap.GetDrawTextureResourcesNumber(),TextureNumBuff,10),
	//	NULL,NULL,
	//};

	//VertexShader.BuildShaders(L"../newDX12Engine/Shader/VertexShader.hlsl", "VertexShaderMain", "vs_5_1", ShaderMacro);
	//PixelShader.BuildShaders(L"../newDX12Engine/Shader/VertexShader.hlsl", "PixelShaderMain", "ps_5_1", ShaderMacro);
	//DirectXPipelineState.BindShader(VertexShader, PixelShader);

	////输入布局
	//InputElementDesc =
	//{
	//	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	//	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	//	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	//	{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	//	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	//};
	//DirectXPipelineState.BindInputLayout(InputElementDesc.data(), InputElementDesc.size());

	
}

void FRenderingPipeline::PreDraw(float DeltaTime)
{
	DirectXPipelineState.PreDraw(DeltaTime);

	RenderLayer.PreDraw(DeltaTime);
}

void FRenderingPipeline::Draw(float DeltaTime)
{
	GeometryMap.PreDraw(DeltaTime);
	RootSignature.PreDraw(DeltaTime);

	GeometryMap.Draw(DeltaTime);
	RenderLayer.Draw(DeltaTime);

	DirectXPipelineState.Draw(DeltaTime);
}

void FRenderingPipeline::PostDraw(float DeltaTime)
{
	GeometryMap.PostDraw(DeltaTime);
	RenderLayer.PostDraw(DeltaTime);
	DirectXPipelineState.PostDraw(DeltaTime);
}
