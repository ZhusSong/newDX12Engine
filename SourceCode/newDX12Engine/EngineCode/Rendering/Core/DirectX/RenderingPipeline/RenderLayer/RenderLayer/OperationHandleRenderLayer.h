#pragma once
//25.8.15  李
#include "../Core/RenderLayer.h"

class FOperationHandleRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FOperationHandleRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();
	virtual void BuildPSO();
	virtual int GetRenderLayerType() const { return 10; }

	virtual void ResetPSO();
};