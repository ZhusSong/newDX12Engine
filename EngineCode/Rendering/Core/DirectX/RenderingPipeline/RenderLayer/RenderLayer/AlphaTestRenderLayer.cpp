#include "AlphaTestRenderLayer.h"
#include "../../Geometry/GeometryMap.h"
#include "../../PipelineState/DirectXPipelineState.h"

FAlphaTestRenderLayer::FAlphaTestRenderLayer()
{
	RenderPriority = 2450;
}


void FAlphaTestRenderLayer::BuildShader()
{
	//构建Shader
	//HLSL
	char TextureNumBuff[10] = { 0 };
	D3D_SHADER_MACRO ShaderMacro[] =
	{
		"TEXTURE2D_MAP_NUM",_itoa(GeometryMap->GetDrawTexture2DResourcesNumber(),TextureNumBuff,10),
		"CUBE_MAP_NUM",_itoa(GeometryMap->GetDrawCubeMapResourcesNumber(),TextureNumBuff,10),
		NULL,NULL,
	};

	VertexShader.BuildShaders(L"../newDX12Engine/Shader/VertexShader.hlsl", "VertexShaderMain", "vs_5_1", ShaderMacro);
	PixelShader.BuildShaders(L"../newDX12Engine/Shader/VertexShader.hlsl", "PixelShaderMain", "ps_5_1", ShaderMacro);
	DirectXPipelineState->BindShader(VertexShader, PixelShader);

	//输入布局
	InputElementDesc =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	DirectXPipelineState->BindInputLayout(InputElementDesc.data(), InputElementDesc.size());
}


void FAlphaTestRenderLayer::BuildPSO()
{
	Super::BuildPSO();
	DirectXPipelineState->Build(AlphaTest);
}

void FAlphaTestRenderLayer::Draw(float DeltaTime)
{
	DirectXPipelineState->ResetPSO(AlphaTest);

	Super::Draw(DeltaTime);
}
