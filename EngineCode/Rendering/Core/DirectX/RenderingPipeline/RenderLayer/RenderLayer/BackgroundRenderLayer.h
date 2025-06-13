#pragma once
#include "../Core/RenderLayer.h"
//25.6.13 李

class FBackgroundRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FBackgroundRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();
	virtual void BuildPSO();
	virtual int GetRenderLayerType() const { return 3; }
};