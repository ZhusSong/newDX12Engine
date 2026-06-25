
#pragma once
#include "../Core/RenderLayer.h"

class FOpaqueRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FOpaqueRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();

	virtual int GetRenderLayerType() const { return 0; }

	virtual void BuildPSO();
	virtual void ResetPSO();
	virtual void ResetPSO(EPipelineState InPipelineState);
};
