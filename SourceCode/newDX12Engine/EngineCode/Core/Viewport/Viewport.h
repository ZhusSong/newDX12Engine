#pragma once
#include "../../Core/Engine.h"

class FViewport
{
public:
	FViewport();

	void ViewportInit();


public:
	void ResetViewport(UINT InWidth, UINT InHeight);
	void ResetScissorRect(UINT InWidth, UINT InHeight);

	virtual void OnResetSize(int InWidth, int InHeight);

	// 和屏幕的视口有关
	// 画面のビューポートに関連
	D3D12_VIEWPORT ViewprotInfo;
	D3D12_RECT ViewprotRect;

	// 定义摄像机位置,UP，Right,Look
	// カメラの位置、Up、Right、Look を定義
	XMFLOAT4X4 ViewMatrix;

	// 可视范围 可视角度
	// 視野範囲および視野角
	XMFLOAT4X4 ProjectMatrix;
};