#include "RenderLayerManager.h"

#include "../../../../../Component/Mesh/Core/MeshComponentType.h"
#include "../../../../../Component/Mesh/Core/MeshComponent.h"
#include "../Geometry/GeometryMap.h"
#include "../Geometry/RenderingData.h"
#include "../../../../../Core/World.h"
#include "../../../../../Actor/Core/ActorObject.h"
#include "../RenderLayer/BufferRenderLayer/NormalBufferRenderLayer.h"

#include "RenderLayer/AlphaTestRenderLayer.h"
#include "RenderLayer/OpaqueRenderLayer.h"
#include "RenderLayer/TransparentRenderLayer.h"
#include "RenderLayer/BackgroundRenderLayer.h"
#include "RenderLayer/OpaqueReflectorRenderLayer.h"
#include "RenderLayer/OpaqueShadowRenderLayer.h"
#include "RenderLayer/SelectRenderLayer.h"
#include "RenderLayer/OperationHandleRotPlaneRenderLayer.h"
#include "RenderLayer/SSAORenderLayer.h"
#include "RenderLayer/SSAOBilateralRenderLayer.h"

#if EDITOR_ENGINE
#include "RenderLayer/OperationHandleRenderLayer.h"
#endif

std::vector<std::shared_ptr<FRenderLayer>> FRenderLayerManager::RenderLayers;

FRenderLayerManager::FRenderLayerManager()
{
	RenderLayers.clear();

#if EDITOR_ENGINE
	CreateRenderLayer<FOperationHandleRenderLayer>();
#endif

	CreateRenderLayer<FOperationHandleRotPlaneRenderLayer>();

	//CreateRenderLayer<FSSAOBilateralRenderLayer>();
	//CreateRenderLayer<FSSAORenderLayer>();
	CreateRenderLayer<FNormalBufferRenderLayer>();
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


// 高亮选择物体
void FRenderLayerManager::HighlightDisplayObject(GActorObject* InObject)
{
	FGeometry::FindRenderingDatas(
		[&](std::shared_ptr<FRenderingData>& InRender)->EFindValueType
		{
			if (GActorObject* InActor = dynamic_cast<GActorObject*>(InRender->Mesh->GetOuter()))
			{
				if (InObject == InActor)
				{
					HighlightDisplayObject(InRender);

					return EFindValueType::TYPE_COMPLETE;
				}
			}

			return EFindValueType::TYPE_IN_PROGRAM;
		});
}

extern int ActorSelected;
void FRenderLayerManager::HighlightDisplayObject(std::weak_ptr<FRenderingData> RenderingData)
{
	//清除旧的物体
	Clear(EMeshRenderLayerType::RENDERLAYER_SELECT);

	//设置新的
	Add(EMeshRenderLayerType::RENDERLAYER_SELECT, RenderingData);

	//记录index
#if EDITOR_ENGINE
	if (GActorObject* InActor = dynamic_cast<GActorObject*>(RenderingData.lock()->Mesh->GetOuter()))
	{
		const vector<GActorObject*>& InActors = GetWorld()->GetActors();
		for (size_t i = 0; i < InActors.size(); i++)
		{
			if (InActors[i] == InActor)
			{
				ActorSelected = i;
				break;
			}
		}
	}
#endif // EDITOR_ENGINE
}

void FRenderLayerManager::HighlightDisplayObject(CComponent* InComponent)
{
	if (CMeshComponent* InMeshComponent = dynamic_cast<CMeshComponent*>(InComponent))
	{
		FGeometry::FindRenderingDatas(
			[&](std::shared_ptr<FRenderingData>& InRender)->EFindValueType
			{
				if (InRender->Mesh == InMeshComponent)
				{
					HighlightDisplayObject(InRender);
					return EFindValueType::TYPE_COMPLETE;
				}

				return EFindValueType::TYPE_IN_PROGRAM;
			});
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

void FRenderLayerManager::OnResetSize(int InWidth, int InHeight)
{
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
