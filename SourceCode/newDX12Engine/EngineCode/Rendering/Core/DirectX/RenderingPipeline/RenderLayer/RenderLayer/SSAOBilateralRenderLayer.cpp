#include "SSAOBilateralRenderLayer.h"
#include "../../Geometry/GeometryMap.h"
#include "../../PipelineState/DirectXPipelineState.h"

namespace
{
	// 仅用于调试 SSAO shader 的快速编译开关
	// SSAO シェーダーをデバッグする際の高速コンパイル用フラグ
	constexpr bool GSSAOFastDebugMode = true;

	UINT GetSSAOShaderCompileFlags()
	{
#if _DEBUG
		return GSSAOFastDebugMode ? 0u : (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
#else
		return 0u;
#endif
	}
}

FSSAOBilateralRenderLayer::FSSAOBilateralRenderLayer()
{
	RenderPriority = 45673;
}

void FSSAOBilateralRenderLayer::BuildShader()
{
	// 构建Shader
	// シェーダーを構築する
	//HLSL
	std::wstring ShaderPath = BuildShadersPaths(L"AO/SSAOBilateralBlur");
	UINT CompileFlags = GetSSAOShaderCompileFlags();
	VertexShader.BuildShaders(ShaderPath, "VertexShaderMain", "vs_5_1", NULL, CompileFlags);
	PixelShader.BuildShaders(ShaderPath, "PixelShaderMain", "ps_5_1", NULL, CompileFlags);
	DirectXPipelineState->BindShader(VertexShader, PixelShader);

	// shader输入格式
	// シェーダー入力フォーマット
	InputElementDesc.clear();
	DirectXPipelineState->BindInputLayout(InputElementDesc.data(), InputElementDesc.size());
}

void FSSAOBilateralRenderLayer::BuildPSO()
{
	Super::BuildPSO();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC& PSOGPSDesc = DirectXPipelineState->GetGPSDesc();
	PSOGPSDesc.DepthStencilState.DepthEnable = false;
	PSOGPSDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	PSOGPSDesc.RTVFormats[0] = DXGI_FORMAT_R16_UNORM;
	PSOGPSDesc.SampleDesc.Count = 1;
	PSOGPSDesc.SampleDesc.Quality = 0;

	PSOGPSDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
	DirectXPipelineState->Build(State_SSAO_Bilateral);
}

void FSSAOBilateralRenderLayer::ResetPSO()
{
	DirectXPipelineState->ResetPSO(State_SSAO_Bilateral);
}

void FSSAOBilateralRenderLayer::Draw(float DeltaTime)
{
	DirectXPipelineState->ResetPSO(State_SSAO_Bilateral);

	GetGraphicsCommandList()->IASetIndexBuffer(nullptr);
	GetGraphicsCommandList()->IASetVertexBuffers(0, 0, nullptr);
	GetGraphicsCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	GetGraphicsCommandList()->DrawInstanced(6, 1, 0, 0);
}
