#include "OperationHandleBase.h"
#include "../../../../EngineCode/Component/Mesh/Core/MeshComponentType.h"
#include "../../../../EngineCode/Component/Mesh/CustomMeshComponent.h"
#include "../../../../EngineCode/Mesh/Core/Material/Material.h"
#include "../../../../EngineCode/Component/InputComponent.h"
#include "../../../../EngineCode/Library/RaycastSystemLibrary.h"
#include "../../../../EngineCode/Core/World.h"
#include "../../../../EngineCode/Core/Camera.h"
#include "../../../../EngineCode/Math/EngineMath.h"

extern CMeshComponent* SelectAxisComponent;

GOperationHandleBase::GOperationHandleBase()
{
	FixedZoom = 50.f;

	FCreateObjectParam Param;
	Param.Outer = this;

	InputComponent = CreateObject<CInputComponent>(Param, new CInputComponent());
}

void GOperationHandleBase::SetMeshRenderLayerType(EMeshRenderLayerType InRenderLayerType)
{
	XAxisComponent->SetMeshRenderLayerType(InRenderLayerType);
	YAxisComponent->SetMeshRenderLayerType(InRenderLayerType);
	ZAxisComponent->SetMeshRenderLayerType(InRenderLayerType);
}

void GOperationHandleBase::SetPosition(const XMFLOAT3& InNewPosition)
{
	Super::SetPosition(InNewPosition);

	XAxisComponent->SetPosition(InNewPosition);
	YAxisComponent->SetPosition(InNewPosition);
	ZAxisComponent->SetPosition(InNewPosition);
}

void GOperationHandleBase::SetScale(const fvector_3d& InNewScale)
{
	Super::SetScale(InNewScale);

	if (InNewScale >= fvector_3d(0.3f))
	{
		if (XAxisComponent && YAxisComponent && ZAxisComponent)
		{
			XAxisComponent->SetScale(InNewScale);
			YAxisComponent->SetScale(InNewScale);
			ZAxisComponent->SetScale(InNewScale);
		}
	}
}

GOperationHandleBase::ESelectAxisType GOperationHandleBase::GetSelectAxis()
{
	if (SelectAxisComponent)
	{
		if (SelectAxisComponent == XAxisComponent)
		{
			return GOperationHandleBase::ESelectAxisType::SELECTAXIS_X;
		}
		else if (SelectAxisComponent == YAxisComponent)
		{
			return GOperationHandleBase::ESelectAxisType::SELECTAXIS_Y;
		}
		else if (SelectAxisComponent == ZAxisComponent)
		{
			return GOperationHandleBase::ESelectAxisType::SELECTAXIS_Z;
		}
	}

	return GOperationHandleBase::ESelectAxisType::SELECTAXIS_NONE;
}

void GOperationHandleBase::ResetVisible(CCustomMeshComponent* InAxisComponent, bool bVisible)
{
	SetVisible(!bVisible);

	InAxisComponent->SetVisible(bVisible);
}

void GOperationHandleBase::ResetVisible()
{
	SetVisible(true);
}

void GOperationHandleBase::ResetColor()
{
	ResetColor(XAxisComponent, fvector_4d(1.f, 0.f, 0.f, 1.f));
	ResetColor(YAxisComponent, fvector_4d(0.f, 1.f, 0.f, 1.f));
	ResetColor(ZAxisComponent, fvector_4d(0.f, 0.f, 1.f, 1.f));
}

void GOperationHandleBase::ResetColor(CCustomMeshComponent* InAxisComponent, const fvector_4d& InColor)
{
	if (InAxisComponent)
	{
		if (CMaterial* InMaterial = (*InAxisComponent->GetMaterials())[0])
		{
			InMaterial->SetBaseColor(InColor);
		}
	}
}

void GOperationHandleBase::BeginInit()
{
	Super::BeginInit();

	InputComponent->OnMouseMoveDelegate.Bind(this, &GOperationHandleBase::OnMouseMove);
	InputComponent->OnLMouseButtonDownDelegate.Bind(this, &GOperationHandleBase::OnLeftMouseButtonDown);
	InputComponent->OnLMouseButtonUpDelegate.Bind(this, &GOperationHandleBase::OnLeftMouseButtonUp);

	SetVisible(false);
}

void GOperationHandleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld())
	{
		if (GetWorld()->GetCamera())
		{
			fvector_3d New3Value = EngineMath::ToVector3d(GetWorld()->GetCamera()->GetPosition()) - EngineMath::ToVector3d(GetPosition());
			fvector_3d Scale = New3Value.len() / FixedZoom;

			SetScale(Scale);
		}
	}
}

void GOperationHandleBase::SetVisible(bool bNewVisible)
{
	XAxisComponent->SetVisible(bNewVisible);
	YAxisComponent->SetVisible(bNewVisible);
	ZAxisComponent->SetVisible(bNewVisible);
}
extern GActorObject* SelectedObject;
extern bool bOperationHandleSelect;
void GOperationHandleBase::OnMouseMove(int X, int Y)
{
	if (!bOperationHandleSelect)
	{
		FCollisionResult CollisionResult;
		FRaycastSystemLibrary::HitSpecificObjectsResultByScreen(
			GetWorld(),
			this,
			X, Y,
			CollisionResult);

		ResetColor();

		if (CollisionResult.bHit)
		{
			CCustomMeshComponent* SelectCustomMeshComponent = dynamic_cast<CCustomMeshComponent*>(CollisionResult.Component);

			SelectAxisComponent = SelectCustomMeshComponent;
			ResetColor(SelectCustomMeshComponent, fvector_4d(1.f, 1.f, 0.f, 1.f));
		}
		else
		{
			SelectAxisComponent = nullptr;

			if (!SelectedObject)
			{
				SetVisible(false);
			}
		}
	}
}

void GOperationHandleBase::OnLeftMouseButtonDown(int X, int Y)
{

}

void GOperationHandleBase::OnLeftMouseButtonUp(int X, int Y)
{

}
