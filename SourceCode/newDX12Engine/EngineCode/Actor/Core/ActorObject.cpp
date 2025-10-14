#include "ActorObject.h"
#include "../../Component/TransformComponent.h"
#include "../../Component/Mesh/Core/MeshComponent.h"
#include "../../Math/EngineMath.h"
#include "../../Core/Construction/MacroConstruction.h"

// Actor对象
GActorObject::GActorObject()
{
	BUILD_OBJECT_PARAMETERS(Type, this);
	RootComponent = CreateObject<CTransformComponent>(ParamType, new CTransformComponent());
}

void GActorObject::GetBoundingBox(BoundingBox& OutBoundingBox)
{
	if (RootComponent)
	{
		fvector_3d MaxPoint = fvector_3d(-FLT_MAX);
		fvector_3d MinPoint = fvector_3d(+FLT_MAX);
		// 得到子类中所有Mesh组件
		std::vector<CComponent*>& InChildrens = RootComponent->GetChildrens();
		for (auto& Tmp : InChildrens)
		{
			if (CMeshComponent* InComponent = dynamic_cast<CMeshComponent*>(Tmp))
			{
				BoundingBox ComponentBoundingBox;
				InComponent->GetBoundingBox(ComponentBoundingBox);

				fvector_3d Center = EngineMath::ToVector3d(ComponentBoundingBox.Center);
				fvector_3d Extents = EngineMath::ToVector3d(ComponentBoundingBox.Extents);

				fvector_3d ComponentMaxPoint = Extents + Center;
				fvector_3d ComponentMinPoint = Center - Extents;


				//提取最大最小值
				MinPoint.x = math_libray::Min(ComponentMinPoint.x, MinPoint.x);
				MinPoint.y = math_libray::Min(ComponentMinPoint.y, MinPoint.y);
				MinPoint.z = math_libray::Min(ComponentMinPoint.z, MinPoint.z);

				MaxPoint.x = math_libray::Max(ComponentMaxPoint.x, MaxPoint.x);
				MaxPoint.y = math_libray::Max(ComponentMaxPoint.y, MaxPoint.y);
				MaxPoint.z = math_libray::Max(ComponentMaxPoint.z, MaxPoint.z);
			}
		}

		// 合并最终AABB
		if (MaxPoint != fvector_3d(-FLT_MAX) ||
			MinPoint != fvector_3d(+FLT_MAX))
		{
			XMFLOAT3 XMFMaxPoint = EngineMath::ToFloat3(MaxPoint);
			XMFLOAT3 XMFMinPoint = EngineMath::ToFloat3(MinPoint);

			XMVECTOR XMFMaxPointTOR = XMLoadFloat3(&XMFMaxPoint);
			XMVECTOR XMFMinPointTOR = XMLoadFloat3(&XMFMinPoint);

			XMStoreFloat3(&OutBoundingBox.Center, (XMFMaxPointTOR + XMFMinPointTOR) * 0.5f);
			XMStoreFloat3(&OutBoundingBox.Extents, (XMFMaxPointTOR - XMFMinPointTOR) * 0.5f);
		}
	}
}

BoundingBox GActorObject::GetBoundingBox()
{
	BoundingBox AABB;
	GetBoundingBox(AABB);

	return AABB;
}
void GActorObject::SetPosition(const XMFLOAT3& InNewPosition)
{
	RootComponent->SetPosition(InNewPosition);
}

void GActorObject::SetRotation(const fvector_3d& InRotation)
{
	RootComponent->SetRotation(InRotation);
}

void GActorObject::SetRotation(const frotator& InNewRotation)
{
	RootComponent->SetRotation(InNewRotation);
}
void GActorObject::SetScale(const fvector_3d& InNewScale)
{
	RootComponent->SetScale(InNewScale);
}

void GActorObject::SetRotationQuat(const fquat& InNewQuatRotation)
{
	RootComponent->SetRotationQuat(InNewQuatRotation);
}


void GActorObject::SetPickup(bool bNewPickup)
{
}
XMFLOAT3& GActorObject::GetPosition()
{
	return RootComponent->GetPosition();
}

fquat GActorObject::GetRotationQuat() const
{
	return RootComponent->GetRotationQuat();
}

XMFLOAT3& GActorObject::GetForwardVector()
{
	return RootComponent->GetForwardVector();
}

XMFLOAT3& GActorObject::GetRightVector()
{
	return RootComponent->GetRightVector();
}

XMFLOAT3& GActorObject::GetUPVector()
{
	return RootComponent->GetUPVector();
}

frotator GActorObject::GetRotation() const
{
	return RootComponent->GetRotation();
}

fvector_3d GActorObject::GetScale() const
{
	return RootComponent->GetScale();
}

