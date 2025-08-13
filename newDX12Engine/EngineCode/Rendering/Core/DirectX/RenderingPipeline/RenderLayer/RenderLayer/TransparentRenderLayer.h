
//25.6.10 李
#pragma once
#include "../Core/RenderLayer.h"

class FTransparentRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FTransparentRenderLayer();

	virtual void Draw(float DeltaTime);
	virtual void BuildShader();
	virtual void BuildPSO();

	virtual int GetRenderLayerType() const { return 1; }
	virtual void ResetPSO();
};