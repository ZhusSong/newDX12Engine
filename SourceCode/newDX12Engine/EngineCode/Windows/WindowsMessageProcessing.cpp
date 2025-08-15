#include "WindowsMessageProcessing.h"
#include "../Component/Input/Input.h"
#include <WindowsX.h>

// 注册IMGUI消息队列
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 操作窗口时，处理操作对应的消息句柄
LRESULT CALLBACK EngineWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 添加IMGUI消息队列
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
	{
		return true;
	}

	// 消息队列
	switch (msg)
	{
		// 关闭
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// 鼠标左键点击
	case WM_LBUTTONDOWN:
		LMouseDownDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
		// 鼠标左键抬起
	case WM_LBUTTONUP:
		LMouseUpDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
		// 鼠标右键点击
	case WM_RBUTTONDOWN:
		MouseDownDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
		// 鼠标右键抬起
	case WM_RBUTTONUP:
		MouseUpDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
		// 鼠标移动
	case WM_MOUSEMOVE:
		MouseMoveDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
		// 鼠标滚轮
	case WM_MOUSEWHEEL:
		MousesWheelsDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (short)HIWORD(wParam));
		return 0;
	}

	//交给windows自己处理完成
	return DefWindowProc(hwnd, msg, wParam, lParam);
}