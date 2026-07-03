#include "SelectRenderLayer.h"
#include "../../Geometry/GeometryMap.h"
#include "../../PipelineState/DirectXPipelineState.h"

FSelectRenderLayer::FSelectRenderLayer()
{
	RenderPriority = 3170;
}

void FSelectRenderLayer::BuildShader()
{
	// 构建Shader
	// シェーダーを構築する
	// HLSL
	vector<ShaderType::FShaderMacro> ShaderMacro;
	BuildShaderMacro(ShaderMacro);

	vector<D3D_SHADER_MACRO> D3DShaderMacro;
	ShaderType::ToD3DShaderMacro(ShaderMacro, D3DShaderMacro);
	std::wstring ShaderPath = BuildShadersPaths(L"SelectShader");
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

void FSelectRenderLayer::BuildPSO()
{
	Super::BuildPSO();

	D3D12_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc;
	RenderTargetBlendDesc.BlendEnable = true;
	RenderTargetBlendDesc.LogicOpEnable = false;

	RenderTargetBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	RenderTargetBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	RenderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	RenderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;//F(1,1,1)
	RenderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;//F(0,0,0)
	RenderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;

	RenderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	RenderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	DirectXPipelineState->SetRenderTarget(0, RenderTargetBlendDesc);

	//// 不进行遮挡剔除
	//// オクルージョンカリングを行わない
	//CD3DX12_DEPTH_STENCIL_DESC DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	//DirectXPipelineState->SetDepthStencilState(DepthStencilDesc);
	
	// 双pass描边
	// ダブルパスのアウトライン描画

	// 关闭深度写入，但保留深度测试
	// 深度書き込みを無効にするが、深度テストは維持する
	CD3DX12_DEPTH_STENCIL_DESC DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	DepthStencilDesc.DepthEnable = TRUE;                        // 启用深度测试         // 深度テストを有効にする
	DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // 不写入深度缓冲    // 深度バッファには書き込まない
	DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;    // 使用正常的深度比较   // 通常の深度比較を使用する
	DirectXPipelineState->SetDepthStencilState(DepthStencilDesc);

	// 渲染背面
	// 背面を描画する
	CD3DX12_RASTERIZER_DESC RasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	RasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;  // 剔除正面，只渲染背面  // 前面をカリングし、背面のみ描画する
	RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	RasterizerDesc.FrontCounterClockwise = FALSE;     // 确保正确的正面定义    // 正しい表面の定義を確保する
	DirectXPipelineState->SetRasterizerState(RasterizerDesc);


	DirectXPipelineState->Build(Select);
}

void FSelectRenderLayer::Draw(float DeltaTime)
{
	DirectXPipelineState->ResetPSO(Select);

	Super::Draw(DeltaTime);
}

bool FSelectRenderLayer::ShouldDrawInCurrentLayer(const FRenderingData& InRenderingData, const CMaterial* InMaterial) const
{
	// The selection pass stores the original mesh in a dedicated layer list,
	// so it must not be filtered back out by the mesh's source render layer.
	return true;
}
