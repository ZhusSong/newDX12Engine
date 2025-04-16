#include "Camera.h"
#include "../Component/InputComponent.h"
#include "../Component/TransformComponent.h"


CCamera::CCamera()
{
	InputComponent = CreateObject<CInputComponent>(new CInputComponent());
	TransformationComponent = CreateObject<CTransformComponent>(new CTransformComponent());
}
void CCamera::BeginInit()
{
	InputComponent->CaptureKeyboardInforDelegate.Bind(this, &CCamera::ExecuteKeyboard);
}
void CCamera::Update(float DeltaTime)
{

}

void CCamera::ExecuteKeyboard(const FInputKey& InputKey)
{

}
