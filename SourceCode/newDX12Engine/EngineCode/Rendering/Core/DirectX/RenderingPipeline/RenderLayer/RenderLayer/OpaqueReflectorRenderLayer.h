#pragma once
//25.6.18 李
#include "../Core/RenderLayer.h"

class FOpaqueReflectorRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FOpaqueReflectorRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();

	virtual int GetRenderLayerType() const { return 4; }

	virtual void BuildPSO();
	virtual void ResetPSO();
};