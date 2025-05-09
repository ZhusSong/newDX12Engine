#include "InputComponent.h"
#include "Input/Input.h"

// 绑定代理函数
void CInputComponent::BeginInit()
{
	MouseDownDelegate.AddFunction(this, &CInputComponent::OnMouseButtonDown);
	MouseUpDelegate.AddFunction(this, &CInputComponent::OnMouseButtonUp);
	MouseMoveDelegate.AddFunction(this, &CInputComponent::OnMouseMove);
	MousesWheelsDelegate.AddFunction(this, &CInputComponent::OnMouseWheel);
}
// 每帧运行
void CInputComponent::Tick(float DeltaTime)
{
	// 检测是否进行了键盘事件的绑定
	if (CaptureKeyboardInforDelegate.IsBound())
	{
		FInputKey InputKey;
		if (GetAsyncKeyState('W') & 0x8000)
		{
			InputKey.KeyName = "W";
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			InputKey.KeyName = "S";
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			InputKey.KeyName = "A";
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			InputKey.KeyName = "D";
		}
		else if (GetAsyncKeyState('Q') & 0x8000)
		{
			InputKey.KeyName = "Q";
		}
		else if (GetAsyncKeyState('E') & 0x8000)
		{
			InputKey.KeyName = "E";
		}
		else
		{
			return;
		}

		CaptureKeyboardInforDelegate.Execute(InputKey);
	}
}

// 鼠标按下时
void CInputComponent::OnMouseButtonDown(int X, int Y)
{
	if (OnMouseButtonDownDelegate.IsBound())
	{
		OnMouseButtonDownDelegate.Execute((int)X, (int)Y);
	}
}

// 鼠标抬起时
void CInputComponent::OnMouseButtonUp(int X, int Y)
{
	if (OnMouseButtonUpDelegate.IsBound())
	{
		OnMouseButtonUpDelegate.Execute((int)X, (int)Y);
	}
}

// 鼠标抬起时
void CInputComponent::OnMouseMove(int X, int Y)
{
	if (OnMouseMoveDelegate.IsBound())
	{
		OnMouseMoveDelegate.Execute((int)X, (int)Y);
	}
}

// 滚轮滚动时
void CInputComponent::OnMouseWheel(int X, int Y, float InDelta)
{
	if (OnMouseWheelDelegate.IsBound())
	{
		OnMouseWheelDelegate.Execute((int)X, (int)Y, (float)InDelta);
	}
}

