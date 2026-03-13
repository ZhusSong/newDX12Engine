#pragma once
#include "../Core/RenderLayer.h"

class FBackgroundRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FBackgroundRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();
	virtual void BuildPSO();
	virtual int GetRenderLayerType() const { return 3; }
	virtual void ResetPSO();
};