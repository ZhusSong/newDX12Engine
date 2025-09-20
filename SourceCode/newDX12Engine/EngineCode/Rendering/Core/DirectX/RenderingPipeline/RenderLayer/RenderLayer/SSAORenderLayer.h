#pragma once
#include "../Core/RenderLayer.h"

class FSSAORenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FSSAORenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();
	virtual void BuildPSO();

	virtual int GetRenderLayerType() const { return 21; }
};