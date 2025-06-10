
//25.6.10 李
#pragma once
#include "../Core/RenderLayer.h"

class FTransparentRenderLayer :public FRenderLayer
{
public:
	FTransparentRenderLayer();

	virtual void BuildShader();

	virtual int GetRenderLayerType() const { return 1; }
};