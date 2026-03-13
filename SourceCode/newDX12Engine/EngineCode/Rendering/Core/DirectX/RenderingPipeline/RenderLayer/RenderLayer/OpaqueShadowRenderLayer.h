#pragma once
#include "../Core/RenderLayer.h"

class FOpaqueShadowRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FOpaqueShadowRenderLayer();

	virtual void Draw(float DeltaTime);

	// 构建普通阴影shader
	// 通常シャドウシェーダーを構築する
	virtual void BuildShader();

	// 构建万向阴影shader
	// 全方向シャドウシェーダーを構築する
	void BuildOmnidirectionalShadowShader();

	virtual int GetRenderLayerType() const { return 8; }

	virtual void BuildPSO();

	virtual void ResetPSO();
	virtual void ResetPSO(EPipelineState InPipelineState);
};