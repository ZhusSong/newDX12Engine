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
	// 绑定代理
	InputComponent->CaptureKeyboardInforDelegate.Bind(this, &CCamera::ExecuteKeyboard);

	InputComponent->OnMouseButtonDownDelegate.Bind(this, &CCamera::OnMouseButtonDown);
	InputComponent->OnMouseButtonUpDelegate.Bind(this, &CCamera::OnMouseButtonUp);
	InputComponent->OnMouseMoveDelegate.Bind(this, &CCamera::OnMouseMove);
	InputComponent->OnMouseWheelDelegate.Bind(this, &CCamera::OnMouseWheel);

}
void CCamera::Update(float DeltaTime)
{

}

void CCamera::ExecuteKeyboard(const FInputKey& InputKey)
{

}

void CCamera::OnMouseButtonDown(int X, int Y)
{
	LastMousePosition.x = X;
	LastMousePosition.y = Y;
}

void CCamera::OnMouseButtonUp(int X, int Y)
{
	
}

void CCamera::OnMouseMove(int X, int Y)
{
	
}

void CCamera::OnMouseWheel(int X, int Y, float InDelta)
{
	
}

void CCamera::MoveForward(float InValue)
{
	
}

void CCamera::MoveRight(float InValue)
{
	
}