#include "InputComponent.h"
#include "Input/Input.h"

// 绑定代理函数
// デリゲート関数をバインド
void CInputComponent::BeginInit()
{
	LMouseDownDelegate.AddFunction(this, &CInputComponent::OnLeftMouseButtonDown);
	LMouseUpDelegate.AddFunction(this, &CInputComponent::OnLeftMouseButtonUp);

	MouseDownDelegate.AddFunction(this, &CInputComponent::OnRightMouseButtonDown);
	MouseUpDelegate.AddFunction(this, &CInputComponent::OnRightMouseButtonUp);

	MouseMoveDelegate.AddFunction(this, &CInputComponent::OnMouseMove);

	MousesWheelsDelegate.AddFunction(this, &CInputComponent::OnMouseWheel);
}
// 每帧运行
// 毎フレーム実行
void CInputComponent::Tick(float DeltaTime)
{
	// 检测是否进行了键盘事件的绑定
	// キーボードイベントがバインドされているかをチェック
	if (CaptureKeyboardInforDelegate.IsBound())
	{
		FInputKey InputKey;
		if (GetAsyncKeyState('W') & KF_UP)
		{
			InputKey.KeyName = "W";
		}
		else if (GetAsyncKeyState('S') & KF_UP)
		{
			InputKey.KeyName = "S";
		}
		else if (GetAsyncKeyState('A') & KF_UP)
		{
			InputKey.KeyName = "A";
		}
		else if (GetAsyncKeyState('D') & KF_UP)
		{
			InputKey.KeyName = "D";
		}
		else if (GetAsyncKeyState('Q') & KF_UP)
		{
			InputKey.KeyName = "Q";
		}
		else if (GetAsyncKeyState('E') & KF_UP)
		{
			InputKey.KeyName = "E";
		}
		else if (GetAsyncKeyState('R') & KF_UP)
		{
			InputKey.KeyName = "R";
		}
		else if (GetAsyncKeyState('F') & KF_UP)
		{
			InputKey.KeyName = "F";
		}
		else if (GetAsyncKeyState(VK_MENU) & KF_UP)//alt
		{
			InputKey.KeyName = "alt";
		}
		else
		{
			return;
		}

		CaptureKeyboardInforDelegate.Execute(InputKey);
	}
}
// 鼠标左键按下时
// マウス左ボタンが押された時
void CInputComponent::OnLeftMouseButtonDown(int X, int Y)
{
	if (OnLMouseButtonDownDelegate.IsBound())
	{
		OnLMouseButtonDownDelegate.Execute((int)X, (int)Y);
	}
}
void CInputComponent::OnLeftMouseButtonUp(int X, int Y)
{
	if (OnLMouseButtonUpDelegate.IsBound())
	{
		OnLMouseButtonUpDelegate.Execute((int)X, (int)Y);
	}
}
// 鼠标右键按下时
void CInputComponent::OnRightMouseButtonDown(int X, int Y)
{
	if (OnRMouseButtonDownDelegate.IsBound())
	{
		OnRMouseButtonDownDelegate.Execute((int)X, (int)Y);
	}
}

// 鼠标抬起时
// マウスボタンを離した時
void CInputComponent::OnRightMouseButtonUp(int X, int Y)
{
	if (OnRMouseButtonUpDelegate.IsBound())
	{
		OnRMouseButtonUpDelegate.Execute((int)X, (int)Y);
	}
}

// 鼠标抬起时
// マウスボタンを離した時
void CInputComponent::OnMouseMove(int X, int Y)
{
	if (OnMouseMoveDelegate.IsBound())
	{
		OnMouseMoveDelegate.Execute((int)X, (int)Y);
	}
}
// 滚轮滚动时
// マウスホイールがスクロールされた時
void CInputComponent::OnMouseWheel(int X, int Y, float InDelta)
{
	if (OnMouseWheelDelegate.IsBound())
	{
		OnMouseWheelDelegate.Execute((int)X, (int)Y, (float)InDelta);
	}
}

