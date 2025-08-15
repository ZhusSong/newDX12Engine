#include "MoveArrow.h"
#include "../../../EngineCode/Component/Mesh/CustomMeshComponent.h"
#include "../../../EngineCode/Core/Construction/MacroConstruction.h"
#include "../../../EngineCode/Library/RaycastSystemLibrary.h"

GMoveArrow::GMoveArrow()
{
	FCreateObjectParam Param;
	Param.Outer = this;

	XAxisComponent = ConstructionObject<CCustomMeshComponent>(Param);
	YAxisComponent = ConstructionObject<CCustomMeshComponent>(Param);
	ZAxisComponent = ConstructionObject<CCustomMeshComponent>(Param);

	SetMeshRenderLayerType(EMeshRenderLayerType::RENDERLAYER_OPERATION_HANDLE);
}

void GMoveArrow::CreateMesh()
{
	string MeshPath = FEnginePathHelper::RelativeToAbsolutePath(FEnginePathHelper::GetEngineAssetPath() + "\\Handle\\MoveArrow.fbx");

	CREATE_RENDER_DATA_BY_COMPONENT(CCustomMeshComponent, XAxisComponent, MeshPath);
	CREATE_RENDER_DATA_BY_COMPONENT(CCustomMeshComponent, YAxisComponent, MeshPath);
	CREATE_RENDER_DATA_BY_COMPONENT(CCustomMeshComponent, ZAxisComponent, MeshPath);

	YAxisComponent->SetRotation(fvector_3d(-90.f, 0.f, 0.f));
	XAxisComponent->SetRotation(fvector_3d(0.f, 90.f, 0.f));

	ResetColor();
}

extern GActorObject* SelectedObject;
extern bool bOperationHandleSelect;
extern CMeshComponent* SelectAxisComponent;

float GMoveArrow::GetMouseScreenMovePosition(
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

			//射线的方向和位置(世界)
			fvector_3d WorldOriginPoint = EngineMath::ToVector3d(WorldOriginPointLOAT3);
			fvector_3d WorldDirection = EngineMath::ToVector3d(WorldDirectionLOAT3);

			WorldDirection.normalize();

			ActorWorldPosition = EngineMath::ToVector3d(SelectedObject->GetPosition());
			if (true)
			{
				//按世界方向
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
				}
			}
			else
			{
				//按对象方向
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
				}
			}

			fvector_3d V1xV2 = fvector_3d::cross_product(WorldDirection, ActorWorldDir);
			float Len = V1xV2.len();

			return (fvector_3d::dot(
				fvector_3d::cross_product(ActorWorldPosition - WorldOriginPoint, WorldDirection),
				V1xV2) / (Len * Len));
		}
	}

	return -1;
}

void GMoveArrow::OnMouseMove(int X, int Y)
{
	Super::OnMouseMove(X, Y);

	if (SelectedObject)
	{
		if (bOperationHandleSelect)
		{
			fvector_3d ActorWorldPosition;
			fvector_3d ActorWorldDir;
			float T1 = GetMouseScreenMovePosition(X, Y, ActorWorldPosition, ActorWorldDir);
			if (T1 != -1)
			{
				fvector_3d WorldMovePosition = ActorWorldDir * T1 + ActorWorldPosition + RelativePosition;

				XMFLOAT3 LOAT3Position = EngineMath::ToFloat3(WorldMovePosition);
				SelectedObject->SetPosition(LOAT3Position);

				SetPosition(LOAT3Position);
			}
		}
	}
}

void GMoveArrow::OnLeftMouseButtonDown(int X, int Y)
{
	Super::OnLeftMouseButtonDown(X, Y);

	if (SelectAxisComponent)
	{
		ResetVisible(dynamic_cast<CCustomMeshComponent*>(SelectAxisComponent), true);

		bOperationHandleSelect = true;

		fvector_3d ActorWorldPosition;
		fvector_3d ActorWorldDir;
		float T1 = GetMouseScreenMovePosition(X, Y, ActorWorldPosition, ActorWorldDir);
		if (T1 != -1)
		{
			fvector_3d WorldMovePosition = ActorWorldDir * T1 + ActorWorldPosition;

			RelativePosition = ActorWorldPosition - WorldMovePosition;
		}
	}
}

void GMoveArrow::OnLeftMouseButtonUp(int X, int Y)
{
	Super::OnLeftMouseButtonUp(X, Y);

	bOperationHandleSelect = false;

	if (SelectAxisComponent)
	{
		ResetVisible();
	}
}
