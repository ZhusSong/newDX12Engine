
//25.6.17 李
#pragma once
#include "../Core/RenderLayer.h"

class FWireframeRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FWireframeRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();

	virtual int GetRenderLayerType() const { return 4; }

	virtual void BuildPSO();
};