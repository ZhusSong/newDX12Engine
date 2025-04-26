#include "ActorObject.h"
#include "../../Component/TransformComponent.h"

GActorObject::GActorObject()
{
	TransformComponent = CreateObject<CTransformComponent>(new CTransformComponent());
}

void GActorObject::SetPosition(const XMFLOAT3& InNewPosition)
{
	TransformComponent->SetPosition(InNewPosition);
}

void GActorObject::SetRotation(const fvector_3d& InRotation)
{
	TransformComponent->SetRotation(InRotation);
}

void GActorObject::SetScale(const fvector_3d& InNewScale)
{
	TransformComponent->SetScale(InNewScale);
}

XMFLOAT3& GActorObject::GetPosition()
{
	return TransformComponent->GetPosition();
}

XMFLOAT3& GActorObject::GetForwardVector()
{
	return TransformComponent->GetForwardVector();
}

XMFLOAT3& GActorObject::GetRightVector()
{
	return TransformComponent->GetRightVector();
}

XMFLOAT3& GActorObject::GetUPVector()
{
	return TransformComponent->GetUPVector();
}

fvector_3d GActorObject::GetRotation() const
{
	return TransformComponent->GetRotation();
}

fvector_3d GActorObject::GetScale() const
{
	return TransformComponent->GetScale();
}

