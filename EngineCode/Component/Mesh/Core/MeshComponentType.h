#pragma once
//25.6.10 李
enum EMeshRenderLayerType
{
	// 不透明
	RENDERLAYER_OPAQUE = 0,
	// 透明
	RENDERLAYER_TRANSPARENT,
	// alpha测试
	RENDERLAYER_ALPHATEST,
	// 背景
	RENDERLAYER_BACKGROUND,
	//// 线框
	//RENDERLAYER_WIREFRAME,
};