#pragma once
enum EMeshRenderLayerType
{
	// 不透明
	// 不透明
	RENDERLAYER_OPAQUE = 0,
	// 透明
	// 透明
	RENDERLAYER_TRANSPARENT,
	// alpha测试
	// アルファテスト
	RENDERLAYER_ALPHATEST,
	// 背景
	// 背景
	RENDERLAYER_BACKGROUND,
	// 反射
	// 反射
	RENDERLAYER_OPAQUE_REFLECTOR = 4,
	// 阴影
	// シャドウ
	RENDERLAYER_SHADOW_RENDER = 8,
	// 选择
	// 選択
	RENDERLAYER_SELECT = 9,
	// 方向箭头
	// 方向矢印
	RENDERLAYER_OPERATION_HANDLE = 10,
	// 旋转面片
	// 回転用平面メッシュ
	RENDERLAYER_OPERATION_HANDLE_ROT_PLANE = 11,
	// SSAO相关:法线渲染层
	// SSAO関連：法線レンダリングレイヤー
	RENDERLAYER_NORMAL = 20,
	// SSAO
	// SSAO
	RENDERLAYER_SSAO = 21,
	// 双边模糊
	// 双方向ブラー
	RENDERLAYER_SSAO_BILATERAL_BLUR = 22,
};