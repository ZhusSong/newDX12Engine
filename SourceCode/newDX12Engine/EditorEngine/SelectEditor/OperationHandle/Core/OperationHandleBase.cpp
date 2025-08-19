#include "OperationHandleBase.h"
#include "../../../../EngineCode/Component/Mesh/Core/MeshComponentType.h"
#include "../../../../EngineCode/Component/Mesh/CustomMeshComponent.h"
#include "../../../../EngineCode/Mesh/Core/Material/Material.h"
#include "../../../../EngineCode/Component/InputComponent.h"
#include "../../../../EngineCode/Library/RaycastSystemLibrary.h"
#include "../../../../EngineCode/Core/World.h"
#include "../../../../EngineCode/Core/Camera.h"
#include "../../../../EngineCode/Math/EngineMath.h"
#include "../../../../Common/OperationHandleSelectManager.h"

extern CMeshComponent* SelectAxisComponent;
extern GActorObject* SelectedObject;
extern bool bOperationHandleSelect;

GOperationHandleBase::GOperationHandleBase()
{
	FixedZoom = 50.f;

	bPressRightMouse = false;

	FCreateObjectParam Param;
	Param.Outer = this;

	InputComponent = CreateObject<CInputComponent>(Param, new CInputComponent());

	XAxisComponent = ConstructionObject<CCustomMeshComponent>(Param);
	YAxisComponent = ConstructionObject<CCustomMeshComponent>(Param);
	ZAxisComponent = ConstructionObject<CCustomMeshComponent>(Param);
	AxisComponent = ConstructionObject<CCustomMeshComponent>(Param);

	SetMeshRenderLayerType(EMeshRenderLayerType::RENDERLAYER_OPERATION_HANDLE);

}

void GOperationHandleBase::SetMeshRenderLayerType(EMeshRenderLayerType InRenderLayerType)
{
	XAxisComponent->SetMeshRenderLayerType(InRenderLayerType);
	YAxisComponent->SetMeshRenderLayerType(InRenderLayerType);
	ZAxisComponent->SetMeshRenderLayerType(InRenderLayerType);

	AxisComponent->SetMeshRenderLayerType(InRenderLayerType);
}

void GOperationHandleBase::SetPosition(const XMFLOAT3& InNewPosition)
{
	Super::SetPosition(InNewPosition);

	XAxisComponent->SetPosition(InNewPosition);
	YAxisComponent->SetPosition(InNewPosition);
	ZAxisComponent->SetPosition(InNewPosition);

	AxisComponent->SetPosition(InNewPosition);
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

			AxisComponent->SetScale(InNewScale);
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
		else if (SelectAxisComponent == AxisComponent)
		{
			return GOperationHandleBase::ESelectAxisType::SELECTAXIS_ANY;
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

	ResetColor(AxisComponent, fvector_4d(0.f, 1.f, 1.f, 1.f)); //绿 + 蓝
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

	InputComponent->CaptureKeyboardInforDelegate.Bind(this, &GOperationHandleBase::OnCaptureKeyboardInfor);
	InputComponent->OnMouseMoveDelegate.Bind(this, &GOperationHandleBase::OnMouseMove);
	
	InputComponent->OnLMouseButtonDownDelegate.Bind(this, &GOperationHandleBase::OnLeftMouseButtonDown);
	InputComponent->OnLMouseButtonUpDelegate.Bind(this, &GOperationHandleBase::OnLeftMouseButtonUp);
	
	InputComponent->OnRMouseButtonDownDelegate.Bind(this, &GOperationHandleBase::OnRMouseButtonDown);
	InputComponent->OnRMouseButtonUpDelegate.Bind(this, &GOperationHandleBase::OnRMouseButtonUp);

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

	AxisComponent->SetVisible(bNewVisible);
}

fvector_3d GOperationHandleBase::AnyAxis(
	const fvector_3d& InRayWorldOriginPoint, 
	const fvector_3d& InRayWorldDirection, 
	const fvector_3d& InActorWorldOriginPoint)
{
	return fvector_3d(0.f);
}


fvector_3d GOperationHandleBase::GetSelectedObjectDirection(
	const fvector_3d& WorldOriginPoint,
	const fvector_3d& WorldDirection,
	const fvector_3d& ActorWorldPosition)
{
	fvector_3d ActorWorldDir;
	GOperationHandleBase::ESelectAxisType AxisType = GetSelectAxis();

	if (true)
	{
		// 按世界方向
		switch (AxisType)
		{
		case GOperationHandleBase::SELECTAXIS_X:
			ActorWorldDir = fvector_3d(1.f, 0.f, 0.f);
			break;
		case GOperationHandleBase::SELECTAXIS_Y:
			ActorWorldDir = fvector_3d(0.f, 1.f, 0.f);
			break;
		case GOperationHandleBase::SELECTAXIS_Z:
			ActorWorldDir = fvector_3d(0.f, 0.f, 1.f);
			break;
		case GOperationHandleBase::SELECTAXIS_ANY:
			ActorWorldDir = AnyAxis(
				WorldOriginPoint,
				WorldDirection,
				ActorWorldPosition);
			break;
		}
	}
	else
	{
		// 按对象方向
		switch (AxisType)
		{
		case GOperationHandleBase::SELECTAXIS_X:
			ActorWorldDir = EngineMath::ToVector3d(SelectedObject->GetRightVector());
			break;
		case GOperationHandleBase::SELECTAXIS_Y:
			ActorWorldDir = EngineMath::ToVector3d(SelectedObject->GetUPVector());
			break;
		case GOperationHandleBase::SELECTAXIS_Z:
			ActorWorldDir = EngineMath::ToVector3d(SelectedObject->GetForwardVector());
			break;
		case GOperationHandleBase::SELECTAXIS_ANY:
			ActorWorldDir = AnyAxis(
				WorldOriginPoint,
				WorldDirection,
				ActorWorldPosition);
			break;
		}
	}

	return ActorWorldDir;
}


void GOperationHandleBase::OnMouseMove(int X, int Y)
{
	if (IsCurrentSelectedHandle())
	{
		if (!bOperationHandleSelect)
		{
			FCollisionResult CollisionResult;
			FRaycastSystemLibrary::HitSpecificObjectsResultByScreen(
				GetWorld(),
				this,
				IgnoreComponents,
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
}

void GOperationHandleBase::OnLeftMouseButtonDown(int X, int Y)
{

}

void GOperationHandleBase::OnLeftMouseButtonUp(int X, int Y)
{

}

void GOperationHandleBase::OnRMouseButtonDown(int X, int Y)
{
	bPressRightMouse = true;
}

void GOperationHandleBase::OnRMouseButtonUp(int X, int Y)
{
	bPressRightMouse = false;
}

void GOperationHandleBase::OnCaptureKeyboardInfor(const FInputKey& InKey)
{
}

float GOperationHandleBase::GetMouseCreenMovePosition(
	int X, int Y,
	fvector_3d& ActorWorldPosition,
	fvector_3d& ActorWorldDir)
{
	if (!SelectedObject)
	{
		return -1;
	}

	GOperationHandleBase::ESelectAxisType AxisType = GetSelectAxis();
	if (AxisType != GOperationHandleBase::ESelectAxisType::SELECTAXIS_NONE)
	{
		XMVECTOR ViewOriginPoint;
		XMVECTOR ViewDirection;
		XMMATRIX ViewInverseMatrix;
		if (FRaycastSystemLibrary::GetRaycastByScreenParam(
			GetWorld(),
			fvector_2id(X, Y),
			ViewOriginPoint,
			ViewDirection,
			ViewInverseMatrix))
		{
			XMVECTOR CTORWorldOriginPoint = XMVector3TransformCoord(ViewOriginPoint, ViewInverseMatrix);
			XMVECTOR CTORWorldDirection = XMVector3TransformNormal(ViewDirection, ViewInverseMatrix);

			XMFLOAT3 WorldOriginPointLOAT3;
			XMFLOAT3 WorldDirectionLOAT3;
			XMStoreFloat3(&WorldOriginPointLOAT3, CTORWorldOriginPoint);
			XMStoreFloat3(&WorldDirectionLOAT3, CTORWorldDirection);

			// 射线的方向和位置(世界)
			fvector_3d WorldOriginPoint = EngineMath::ToVector3d(WorldOriginPointLOAT3);
			fvector_3d WorldDirection = EngineMath::ToVector3d(WorldDirectionLOAT3);

			WorldDirection.normalize();

			ActorWorldPosition = EngineMath::ToVector3d(SelectedObject->GetPosition());

			ActorWorldDir = GetSelectedObjectDirection(
				WorldOriginPoint,
				WorldDirection,
				ActorWorldPosition);

			fvector_3d V1xV2 = fvector_3d::cross_product(WorldDirection, ActorWorldDir);
			float Len = V1xV2.len();

			return (fvector_3d::dot(
				fvector_3d::cross_product(ActorWorldPosition - WorldOriginPoint, WorldDirection),
				V1xV2) / (Len * Len));
		}
	}

	return -1;
}


bool GOperationHandleBase::IsCurrentSelectedHandle()
{
	return this == FOperationHandleSelectManager::Get()->GetSelectedOperationHandle();
}

