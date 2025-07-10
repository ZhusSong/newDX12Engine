#include "RenderLayerManager.h"
#include "RenderLayer/AlphaTestRenderLayer.h"
#include "RenderLayer/OpaqueRenderLayer.h"
#include "RenderLayer/TransparentRenderLayer.h"
#include "RenderLayer/BackgroundRenderLayer.h"
#include "RenderLayer/OpaqueReflectorRenderLayer.h"
#include "RenderLayer/OpaqueShadowRenderLayer.h"
#include "RenderLayer/SelectRenderLayer.h"

std::vector<std::shared_ptr<FRenderLayer>> FRenderLayerManager::RenderLayers;

FRenderLayerManager::FRenderLayerManager()
{
	RenderLayers.clear();

	CreateRenderLayer<FSelectRenderLayer>();
	CreateRenderLayer<FOpaqueShadowRenderLayer>();
	CreateRenderLayer<FBackgroundRenderLayer>();
	CreateRenderLayer<FOpaqueRenderLayer>();
	CreateRenderLayer<FTransparentRenderLayer>();
	CreateRenderLayer<FOpaqueReflectorRenderLayer>();
}

FRenderLayerManager::~FRenderLayerManager()
{
	RenderLayers.clear();
}

void FRenderLayerManager::Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState)
{
	for (auto &Tmp : RenderLayers)
	{
		Tmp->Init(InGeometryMap, InDirectXPipelineState);
	}
}

void FRenderLayerManager::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	for (auto& Tmp : RenderLayers)
	{
		Tmp->UpdateCalculations(DeltaTime,ViewportInfo);
	}
}

void FRenderLayerManager::BuildPSO()
{
	for (auto& Tmp : RenderLayers)
	{
		Tmp->BuildPSO();
	}

}
void FRenderLayerManager::Sort()
{
	auto CompRenderLayer = 
	[&](const std::shared_ptr<FRenderLayer> &InA,const std::shared_ptr<FRenderLayer> &InB)->bool
	{
		return InA->GetRenderPriority() < InB->GetRenderPriority();
	};

	std::sort(RenderLayers.begin(), RenderLayers.end(),CompRenderLayer);
}
void FRenderLayerManager::ResetPSO(int InLayer)
{
	if (auto InRenderLayer = FindByRenderLayer(InLayer))
	{
		InRenderLayer->ResetPSO();
	}
}

void FRenderLayerManager::ResetPSO(int InLayer, EPipelineState InPipelineState)
{
	if (auto InRenderLayer = FindByRenderLayer(InLayer))
	{
		InRenderLayer->ResetPSO(InPipelineState);
	}
}
void FRenderLayerManager::DrawMesh(float DeltaTime, int InLayer, ERenderingConditions RC)
{
	if (auto InRenderLayer = FindByRenderLayer(InLayer))
	{
		InRenderLayer->DrawMesh(DeltaTime, RC);
	}
}

std::shared_ptr<FRenderLayer> FRenderLayerManager::FindByRenderLayer(int InRenderLayer)
{
	for (auto &Tmp : RenderLayers)
	{
		if (Tmp->GetRenderLayerType() == InRenderLayer)
		{
			return Tmp;
		}
	}

	return NULL;
}

void FRenderLayerManager::Add(int InLayer, std::weak_ptr<FRenderingData> InRenderingData)
{
	if (auto Layer = FindByRenderLayer(InLayer))
	{
		Layer->Add(InRenderingData);
	}
}

void FRenderLayerManager::Remove(int InLayer, std::weak_ptr<FRenderingData> InRenderingData)
{
	if (auto Layer = FindByRenderLayer(InLayer))
	{
		Layer->Remove(InRenderingData);
	}
}

void FRenderLayerManager::Clear(int InLayer)
{
	if (auto Layer = FindByRenderLayer(InLayer))
	{
		Layer->Clear();
	}
}
void FRenderLayerManager::PreDraw(float DeltaTime)
{
	for (auto& Tmp : RenderLayers)
	{
		Tmp->PreDraw(DeltaTime);
	}
}

void FRenderLayerManager::Draw(float DeltaTime)
{
	for (auto& Tmp : RenderLayers)
	{
		Tmp->Draw(DeltaTime);
	}
}

void FRenderLayerManager::PostDraw(float DeltaTime)
{
	for (auto& Tmp : RenderLayers)
	{
		Tmp->PostDraw(DeltaTime);
	}
}

void FRenderLayerManager::Draw(int InLayer, float DeltaTime)
{
	if (auto InRenderLayer = FindByRenderLayer(InLayer))
	{
		InRenderLayer->Draw(DeltaTime);
	}
}
void FRenderLayerManager::FindObjectDraw(float DeltaTime, int InLayer, const CMeshComponent* InKey)
{
	if (auto InRenderLayer = FindByRenderLayer(InLayer))
	{
		InRenderLayer->FindObjectDraw(DeltaTime, InKey);
	}
}
