#include "TransparentRenderLayer.h"
#include "../../PipelineState/DirectXPipelineState.h"
#include "../../Geometry/GeometryMap.h"
#include "../RenderLayerManager.h"
#include "../../../../../../Component/Mesh/Core/MeshComponentType.h"
#include "../../../../../../Core/Viewport/ViewportInfo.h"

#include <algorithm>

FTransparentRenderLayer::FTransparentRenderLayer()
	:CachedViewPosition(0.f, 0.f, 0.f, 1.f)
{
	RenderPriority = 3000;
}

void FTransparentRenderLayer::Draw(float DeltaTime)
{
	ResetPSO();

	std::vector<std::weak_ptr<FRenderingData>> SortedRenderDatas = RenderDatas;
	for (const std::shared_ptr<FRenderLayer>& InRenderLayer : FRenderLayerManager::GetRenderLayers())
	{
		if (InRenderLayer &&
			InRenderLayer->GetRenderLayerType() != (int)EMeshRenderLayerType::RENDERLAYER_TRANSPARENT &&
			InRenderLayer->GetRenderLayerType() != (int)EMeshRenderLayerType::RENDERLAYER_OPAQUE_REFLECTOR)
		{
			const std::vector<std::weak_ptr<FRenderingData>>& OtherRenderDatas = InRenderLayer->GetRenderDatas();
			SortedRenderDatas.insert(SortedRenderDatas.end(), OtherRenderDatas.begin(), OtherRenderDatas.end());
		}
	}

	std::sort(SortedRenderDatas.begin(), SortedRenderDatas.end(),
		[&](const std::weak_ptr<FRenderingData>& InA, const std::weak_ptr<FRenderingData>& InB)
		{
			if (InA.expired())
			{
				return false;
			}

			if (InB.expired())
			{
				return true;
			}

			const std::shared_ptr<FRenderingData> RenderingDataA = InA.lock();
			const std::shared_ptr<FRenderingData> RenderingDataB = InB.lock();
			if (!RenderingDataA || !RenderingDataB)
			{
				return RenderingDataA != nullptr;
			}

			const XMFLOAT3 PositionA(
				RenderingDataA->WorldMatrix._41,
				RenderingDataA->WorldMatrix._42,
				RenderingDataA->WorldMatrix._43);
			const XMFLOAT3 PositionB(
				RenderingDataB->WorldMatrix._41,
				RenderingDataB->WorldMatrix._42,
				RenderingDataB->WorldMatrix._43);

			const float DistanceA =
				(PositionA.x - CachedViewPosition.x) * (PositionA.x - CachedViewPosition.x) +
				(PositionA.y - CachedViewPosition.y) * (PositionA.y - CachedViewPosition.y) +
				(PositionA.z - CachedViewPosition.z) * (PositionA.z - CachedViewPosition.z);

			const float DistanceB =
				(PositionB.x - CachedViewPosition.x) * (PositionB.x - CachedViewPosition.x) +
				(PositionB.y - CachedViewPosition.y) * (PositionB.y - CachedViewPosition.y) +
				(PositionB.z - CachedViewPosition.z) * (PositionB.z - CachedViewPosition.z);

			return DistanceA > DistanceB;
		});

	for (std::weak_ptr<FRenderingData>& InRenderingData : SortedRenderDatas)
	{
		DrawObject(DeltaTime, InRenderingData);
	}
}

void FTransparentRenderLayer::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	Super::UpdateCalculations(DeltaTime, ViewportInfo);
	CachedViewPosition = ViewportInfo.ViewPosition;
}

void FTransparentRenderLayer::BuildShader()
{
	// 构建Shader
	// シェーダーを構築する
	vector<ShaderType::FShaderMacro> ShaderMacro;
	BuildShaderMacro(ShaderMacro);

	vector<D3D_SHADER_MACRO> D3DShaderMacro;
	ShaderType::ToD3DShaderMacro(ShaderMacro, D3DShaderMacro);

	std::wstring ShaderPath = BuildShadersPaths(L"VertexShader");
	VertexShader.BuildShaders(ShaderPath, "VertexShaderMain", "vs_5_1", D3DShaderMacro.data());
	PixelShader.BuildShaders(ShaderPath, "PixelShaderMain", "ps_5_1", D3DShaderMacro.data());

	DirectXPipelineState->BindShader(VertexShader, PixelShader);

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

void FTransparentRenderLayer::BuildPSO()
{
	Super::BuildPSO();
	D3D12_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc;
	RenderTargetBlendDesc.BlendEnable = true;
	RenderTargetBlendDesc.LogicOpEnable = false;

	RenderTargetBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	RenderTargetBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	RenderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	RenderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	RenderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	RenderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;

	RenderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	RenderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	CD3DX12_DEPTH_STENCIL_DESC DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	DirectXPipelineState->SetDepthStencilState(DepthStencilDesc);

	DirectXPipelineState->SetRenderTarget(0, RenderTargetBlendDesc);
	DirectXPipelineState->Build(Transparent);
}

void FTransparentRenderLayer::ResetPSO()
{
	DirectXPipelineState->ResetPSO(Transparent);
}
