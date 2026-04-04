#include "WindowsMessageProcessing.h"
#include "../Component/Input/Input.h"
#include <WindowsX.h>

// 注册IMGUI消息队列
// IMGUIメッセージキューを登録
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 操作窗口时，处理操作对应的消息句柄
// ウィンドウ操作時、操作に対応するメッセージハンドルを処理
LRESULT CALLBACK EngineWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 添加IMGUI消息队列
	// IMGUIメッセージキューを追加
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
	{
		return true;
	}

	// 消息队列
	// メッセージキュー
	switch (msg)
	{
		// 关闭
		// 閉じる
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// 鼠标左键点击
		// マウス左クリック
	case WM_LBUTTONDOWN:
		LMouseDownDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
		// 鼠标左键抬起
		// マウス左ボタンアップ
	case WM_LBUTTONUP:
		LMouseUpDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
		// 鼠标右键点击
		// マウス右クリック
	case WM_RBUTTONDOWN:
		MouseDownDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
		// 鼠标右键抬起
		// マウス右ボタンアップ
	case WM_RBUTTONUP:
		MouseUpDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
		// 鼠标移动
		// マウス移動
	case WM_MOUSEMOVE:
		MouseMoveDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
		// 鼠标滚轮
		// マウスホイール
	case WM_MOUSEWHEEL:
		MousesWheelsDelegate.Broadcast(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (short)HIWORD(wParam));
		return 0;
	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED)
		{
			return 0;
		}

		int ViewportWidth = LOWORD(lParam);
		int ViewportHeight = HIWORD(lParam);

		if (ViewportWidth > 0 && ViewportHeight > 0)
		{
			OnResetSizeDelegate.Broadcast(ViewportWidth, ViewportHeight);
		}

		return 0;
	}
	case WM_DPICHANGED:
	{
		const RECT* SuggestedRect = reinterpret_cast<const RECT*>(lParam);
		if (SuggestedRect)
		{
			SetWindowPos(
				hwnd,
				NULL,
				SuggestedRect->left,
				SuggestedRect->top,
				SuggestedRect->right - SuggestedRect->left,
				SuggestedRect->bottom - SuggestedRect->top,
				SWP_NOZORDER | SWP_NOACTIVATE);
		}

		return 0;
	}

	}


	return DefWindowProc(hwnd, msg, wParam, lParam);
}
