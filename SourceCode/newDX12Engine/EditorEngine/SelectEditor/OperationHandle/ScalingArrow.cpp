#include "ScalingArrow.h"
#include "../../../EngineCode/Component/Mesh/CustomMeshComponent.h"
#include "../../../EngineCode/Core/Construction/MacroConstruction.h"
#include "../../../EngineCode/Library/RaycastSystemLibrary.h"
#include "../../../Common/OperationHandleSelectManager.h"

extern GActorObject* SelectedObject;
extern bool bOperationHandleSelect;
extern CMeshComponent* SelectAxisComponent;

GScalingArrow::GScalingArrow()
{
	LastT2Value = 0.f;
}

void GScalingArrow::CreateMesh()
{
	string MeshPath = FEnginePathHelper::RelativeToAbsolutePath(
		FEnginePathHelper::GetEngineAssetPath() + "\\Handle\\ScalingArrow.fbx");

	string AnyAxisMeshPath = FEnginePathHelper::RelativeToAbsolutePath(
		FEnginePathHelper::GetEngineAssetPath() + "\\Handle\\AnyAxis_Type_1.fbx");

	CREATE_RENDER_DATA_BY_COMPONENT(CCustomMeshComponent, XAxisComponent, MeshPath);
	CREATE_RENDER_DATA_BY_COMPONENT(CCustomMeshComponent, YAxisComponent, MeshPath);
	CREATE_RENDER_DATA_BY_COMPONENT(CCustomMeshComponent, ZAxisComponent, MeshPath);
	CREATE_RENDER_DATA_BY_COMPONENT(CCustomMeshComponent, AxisComponent, AnyAxisMeshPath);

	YAxisComponent->SetRotation(fvector_3d(-90.f, 0.f, 0.f));
	XAxisComponent->SetRotation(fvector_3d(0.f, 90.f, 0.f));

	ResetColor();
}

fvector_3d GScalingArrow::AnyAxis(const fvector_3d& InRayWorldOriginPoint, const fvector_3d& InRayWorldDirection, const fvector_3d& InActorWorldOriginPoint)
{
	return fvector_3d(1.f);
}

void GScalingArrow::OnMouseMove(int X, int Y)
{
	Super::OnMouseMove(X, Y);

	if (IsCurrentSelectedHandle())
	{
		if (SelectedObject)
		{
			if (bOperationHandleSelect)
			{
				fvector_3d ActorWorldPosition;
				fvector_3d ActorWorldDir;
				float T2 = GetMouseCreenMovePosition(X, Y, ActorWorldPosition, ActorWorldDir);
				if (T2 != -1)
				{
					fvector_3d CurrentScale = SelectedObject->GetScale();

					float InValueOffset = T2 - LastT2Value;

					//Engine_Log("InValueOffset =%f", InValueOffset)

					float Value = 0.f;
					if (InValueOffset < 0.f)
					{
						Value = -0.25f;
					}
					else if (InValueOffset > 0.f)
					{
						Value = 0.25f;
					}

					CurrentScale += (ActorWorldDir * Value);

					SelectedObject->SetScale(CurrentScale);

					LastT2Value = T2;
				}
			}
		}
	}
}

void GScalingArrow::OnLeftMouseButtonDown(int X, int Y)
{
	Super::OnLeftMouseButtonDown(X, Y);

	if (IsCurrentSelectedHandle())
	{
		if (SelectAxisComponent)
		{
			ResetVisible(dynamic_cast<CCustomMeshComponent*>(SelectAxisComponent), true);

			bOperationHandleSelect = true;

			fvector_3d ActorWorldPosition;
			fvector_3d ActorWorldDir;
			float T2 = GetMouseCreenMovePosition(X, Y, ActorWorldPosition, ActorWorldDir);
			if (T2 != -1)
			{
				LastT2Value = T2;
			}
		}
	}
}

void GScalingArrow::OnLeftMouseButtonUp(int X, int Y)
{
	Super::OnLeftMouseButtonUp(X, Y);

	if (IsCurrentSelectedHandle())
	{
		bOperationHandleSelect = false;

		if (SelectAxisComponent)
		{
			ResetVisible();
		}
	}
}

void GScalingArrow::OnCaptureKeyboardInfor(const FInputKey& InKey)
{
	if (SelectObject)
	{
		if (InKey.KeyName == "R")
		{
			FOperationHandleSelectManager::Get()->DisplaySelectedOperationHandle(this);
		}
	}
}
