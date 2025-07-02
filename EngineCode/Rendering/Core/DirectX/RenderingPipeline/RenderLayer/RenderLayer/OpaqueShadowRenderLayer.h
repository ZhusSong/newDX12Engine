
//25.6.25 李
#pragma once
#include "../Core/RenderLayer.h"

class FOpaqueShadowRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FOpaqueShadowRenderLayer();

	virtual void Draw(float DeltaTime);

	// 构建普通阴影shader
	virtual void BuildShader();

	// 构建万向阴影shader
	void BuildOmnidirectionalShadowShader();

	virtual int GetRenderLayerType() const { return 8; }

	virtual void BuildPSO();

	virtual void ResetPSO();
	virtual void ResetPSO(EPipelineState InPipelineState);
};