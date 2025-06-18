#include "RenderLayerManager.h"
#include "RenderLayer/AlphaTestRenderLayer.h"
#include "RenderLayer/OpaqueRenderLayer.h"
#include "RenderLayer/TransparentRenderLayer.h"
#include "RenderLayer/BackgroundRenderLayer.h"
#include "RenderLayer/OpaqueReflectorRenderLayer.h"

std::vector<std::shared_ptr<FRenderLayer>> FRenderLayerManager::RenderLayers;

FRenderLayerManager::FRenderLayerManager()
{
	RenderLayers.clear();

	CreateRenderLayer<FBackgroundRenderLayer>();
	//CreateRenderLayer<FAlphaTestRenderLayer>();
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
	for (auto& Tmp : RenderLayers)
	{
		if (Tmp->GetRenderLayerType() == InLayer)
		{
			Tmp->Draw(DeltaTime);
			break;
		}
	}
}
void FRenderLayerManager::FindObjectDraw(float DeltaTime, int InLayer, const CMeshComponent* InKey)
{
	for (auto& Tmp : RenderLayers)
	{
		if (Tmp->GetRenderLayerType() == InLayer)
		{
			Tmp->FindObjectDraw(DeltaTime, InKey);
			break;
		}
	}
}
