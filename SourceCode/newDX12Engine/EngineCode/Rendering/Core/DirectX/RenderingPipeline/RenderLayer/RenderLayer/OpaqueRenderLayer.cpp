#include "OpaqueRenderLayer.h"
#include "../../Geometry/GeometryMap.h"
#include "../../PipelineState/DirectXPipelineState.h"

FOpaqueRenderLayer::FOpaqueRenderLayer()
{
	RenderPriority = 2000;
}

void FOpaqueRenderLayer::Draw(float DeltaTime)
{
	// 重置PSO
	// 現在のPSOをリセットする
	ResetPSO();

	Super::Draw(DeltaTime);
}
void FOpaqueRenderLayer::BuildShader()
{
	//　构建Shader
	// シェーダーを構築する
	//HLSL
	vector<ShaderType::FShaderMacro> ShaderMacro;
	BuildShaderMacro(ShaderMacro);

	vector<D3D_SHADER_MACRO> D3DShaderMacro;
	ShaderType::ToD3DShaderMacro(ShaderMacro, D3DShaderMacro);
	//VertexShader.BuildShaders(L"../newDX12Engine/x64/Debug/Shader/VertexShader.hlsl", "VertexShaderMain", "vs_5_1", D3DShaderMacro.data());
	//PixelShader.BuildShaders(L"../newDX12Engine/x64/Debug/Shader/VertexShader.hlsl", "PixelShaderMain", "ps_5_1", D3DShaderMacro.data());

	/*VertexShader.BuildShaderByName(L"VertexShader.hlsl", "VertexShaderMain", "vs_5_1", D3DShaderMacro.data());
	PixelShader.BuildShaderByName(L"VertexShader.hlsl", "PixelShaderMain", "ps_5_1", D3DShaderMacro.data());*/

	std::wstring ShaderPath = BuildShadersPaths(L"VertexShader");
	VertexShader.BuildShaders(ShaderPath, "VertexShaderMain", "vs_5_1", D3DShaderMacro.data());
	PixelShader.BuildShaders(ShaderPath, "PixelShaderMain", "ps_5_1", D3DShaderMacro.data());

	DirectXPipelineState->BindShader(VertexShader, PixelShader);

	


	// shader输入格式
	// シェーダー入力フォーマット
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

void FOpaqueRenderLayer::BuildPSO()
{
	Super::BuildPSO();
	// 构建管线
	// パイプラインを構築する

	// 构建固体模式
	// ソリッドモードを構築する
	DirectXPipelineState->Build(GrayModel);

	// 构建线框
	// ワイヤーフレームを構築する
	DirectXPipelineState->SetFillMode(true);
	DirectXPipelineState->Build(Wireframe);
}

void FOpaqueRenderLayer::ResetPSO()
{
	DirectXPipelineState->ResetPSO();
}
