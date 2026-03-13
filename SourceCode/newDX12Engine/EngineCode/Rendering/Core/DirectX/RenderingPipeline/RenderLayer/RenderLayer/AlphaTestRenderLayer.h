
#pragma once
#include "../Core/RenderLayer.h"

class FAlphaTestRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FAlphaTestRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();
	virtual void BuildPSO();
	virtual int GetRenderLayerType() const { return 2; }
};