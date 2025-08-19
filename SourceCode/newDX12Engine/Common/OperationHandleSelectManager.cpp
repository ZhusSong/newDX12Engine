#include "OperationHandleSelectManager.h"

#if EDITOR_ENGINE

#include "../EditorEngine/SelectEditor/OperationHandle/MoveArrow.h"
#include "../EditorEngine/SelectEditor/OperationHandle/RotatorArrow.h"
#include "../EditorEngine/SelectEditor/OperationHandle/ScalingArrow.h"

extern class GMoveArrow* MoveArrow;
extern class GScalingArrow* ScalingArrow;
extern class GRotatorArrow* RotatorArrow;
extern class GActorObject* SelectedObject;

FOperationHandleSelectManager* FOperationHandleSelectManager::InManage = nullptr;

FOperationHandleSelectManager::FOperationHandleSelectManager()
{

}

FOperationHandleSelectManager* FOperationHandleSelectManager::Get()
{
	if (!InManage)
	{
		InManage = new FOperationHandleSelectManager();
	}

	return InManage;
}

void FOperationHandleSelectManager::Destroy()
{
	if (InManage)
	{
		delete InManage;
		InManage = nullptr;
	}
}

void FOperationHandleSelectManager::AllOperationHandleHide()
{
	MoveArrow->SetVisible(false);
	ScalingArrow->SetVisible(false);
	RotatorArrow->SetVisible(false);
}

void FOperationHandleSelectManager::DisplaySelectedOperationHandle(GActorObject* InNewSelectedObject)
{
	if (SelectedObject)
	{
		// 隐藏所有手柄
		AllOperationHandleHide();

		// 操作手柄附加到对象身上
		if (GOperationHandleBase* InHandleBase = dynamic_cast<GOperationHandleBase*>(InNewSelectedObject))
		{
			InHandleBase->SetPosition(SelectedObject->GetPosition());
			//InHandleBase->SetRotation(SelectedObject->GetRotation());
			InHandleBase->SetVisible(true);
		}

		// 记录操作手柄
		SetNewSelectedOperationHandle(InNewSelectedObject);
	}
}

void FOperationHandleSelectManager::DisplaySelectedOperationHandle()
{
	if (SelectedObject)
	{
		if (!SelectedOperationHandle)
		{
			if (MoveArrow)
			{
				MoveArrow->SetPosition(SelectedObject->GetPosition());
				MoveArrow->SetVisible(true);

				SetNewSelectedOperationHandle(MoveArrow);
			}
		}
		else
		{
			if (GOperationHandleBase* InHandleBase = dynamic_cast<GOperationHandleBase*>(SelectedOperationHandle))
			{
				InHandleBase->SetPosition(SelectedObject->GetPosition());
				InHandleBase->SetVisible(true);
			}
		}
	}
}

void FOperationHandleSelectManager::HideSelectedOperationHandle()
{
	if (GOperationHandleBase* InHandleBase = dynamic_cast<GOperationHandleBase*>(SelectedOperationHandle))
	{
		InHandleBase->SetVisible(false);
	}
}

void FOperationHandleSelectManager::SetNewSelectedOperationHandle(GActorObject* InNewSelectedObject)
{
	SelectedOperationHandle = InNewSelectedObject;
}

void FOperationHandleSelectManager::SetNewSelectedObject(GActorObject* InNewSelectedObject)
{
	SelectedObject = InNewSelectedObject;
}

GActorObject* FOperationHandleSelectManager::GetSelectedOperationHandle()
{
	return SelectedOperationHandle;
}

#endif