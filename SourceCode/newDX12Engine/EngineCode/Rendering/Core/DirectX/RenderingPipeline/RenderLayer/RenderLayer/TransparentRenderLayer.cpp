#include "TransparentRenderLayer.h"
#include "../../PipelineState/DirectXPipelineState.h"
#include "../../Geometry/GeometryMap.h"
#include "../RenderLayerManager.h"
#include "../../../../../../Mesh/Core/Material/Material.h"
#include "../../../../../../Component/Mesh/Core/MeshComponent.h"
#include "../../../../../../Component/Mesh/Core/MeshComponentType.h"
#include "../../../../../../Core/Viewport/ViewportInfo.h"

#include <algorithm>

namespace
{
	float BuildDistanceSquared(const std::shared_ptr<FRenderingData>& InRenderingData, const XMFLOAT4& InViewPosition)
	{
		const XMFLOAT3 Position(
			InRenderingData->WorldMatrix._41,
			InRenderingData->WorldMatrix._42,
			InRenderingData->WorldMatrix._43);

		return
			(Position.x - InViewPosition.x) * (Position.x - InViewPosition.x) +
			(Position.y - InViewPosition.y) * (Position.y - InViewPosition.y) +
			(Position.z - InViewPosition.z) * (Position.z - InViewPosition.z);
	}

	bool IsGlassTransparentCandidate(const std::weak_ptr<FRenderingData>& InWeakRenderingData)
	{
		if (InWeakRenderingData.expired())
		{
			return false;
		}

		if (const std::shared_ptr<FRenderingData> InRenderingData = InWeakRenderingData.lock())
		{
			if (CMaterial* Material = InRenderingData->Mesh->GetMaterialBySlot(InRenderingData->MaterialSlotIndex))
			{
				return Material->IsUseGlass();
			}
		}

		return false;
	}
}

FTransparentRenderLayer::FTransparentRenderLayer()
	:CachedViewPosition(0.f, 0.f, 0.f, 1.f)
{
	RenderPriority = 3000;
}

void FTransparentRenderLayer::Draw(float DeltaTime)
{
	ResetPSO();
	DrawSortedRenderDatas(DeltaTime);
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

	CD3DX12_RASTERIZER_DESC RasterizerDesc(D3D12_DEFAULT);
	RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	RasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	DirectXPipelineState->SetRasterizerState(RasterizerDesc);
	DirectXPipelineState->Build(PlanarTransparent);
}

void FTransparentRenderLayer::ResetPSO()
{
	DirectXPipelineState->ResetPSO(Transparent);
}

void FTransparentRenderLayer::ResetPSO(EPipelineState InPipelineState)
{
	DirectXPipelineState->ResetPSO(InPipelineState);
}

void FTransparentRenderLayer::DrawWithPipelineState(float DeltaTime, EPipelineState InPipelineState)
{
	ResetPSO(InPipelineState);
	DrawSortedRenderDatas(DeltaTime);
}

std::vector<std::weak_ptr<FRenderingData>> FTransparentRenderLayer::BuildSortedRenderDatas() const
{
	std::vector<std::weak_ptr<FRenderingData>> SortedRenderDatas = RenderDatas;
	for (const std::shared_ptr<FRenderLayer>& InRenderLayer : FRenderLayerManager::GetRenderLayers())
	{
		if (InRenderLayer == nullptr ||
			InRenderLayer->GetRenderLayerType() == (int)EMeshRenderLayerType::RENDERLAYER_TRANSPARENT)
		{
			continue;
		}

		for (const std::weak_ptr<FRenderingData>& OtherRenderingData : InRenderLayer->GetRenderDatas())
		{
			if (IsGlassTransparentCandidate(OtherRenderingData))
			{
				SortedRenderDatas.push_back(OtherRenderingData);
			}
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

			const float DistanceA = BuildDistanceSquared(RenderingDataA, CachedViewPosition);
			const float DistanceB = BuildDistanceSquared(RenderingDataB, CachedViewPosition);

			return DistanceA > DistanceB;
		});

	return SortedRenderDatas;
}

void FTransparentRenderLayer::DrawSortedRenderDatas(float DeltaTime)
{
	std::vector<std::weak_ptr<FRenderingData>> SortedRenderDatas = BuildSortedRenderDatas();
	for (std::weak_ptr<FRenderingData>& InRenderingData : SortedRenderDatas)
	{
		DrawObject(DeltaTime, InRenderingData);
	}
}
