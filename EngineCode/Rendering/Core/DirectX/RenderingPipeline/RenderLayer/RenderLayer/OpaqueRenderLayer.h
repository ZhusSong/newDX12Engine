
//25.6.10 李
#pragma once
#include "../Core/RenderLayer.h"

class FOpaqueRenderLayer :public FRenderLayer
{
public:
	FOpaqueRenderLayer();

	virtual void BuildShader();

	virtual int GetRenderLayerType() const { return 0; }
};