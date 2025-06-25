
//25.6.25 李
#pragma once
#include "../Core/RenderLayer.h"

class FOpaqueShadowRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FOpaqueShadowRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();

	virtual int GetRenderLayerType() const { return 8; }

	virtual void BuildPSO();

	virtual void ResetPSO();
};