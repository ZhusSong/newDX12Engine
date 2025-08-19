#pragma once
//25.8.19  李
#include "../Core/RenderLayer.h"

// 平面旋转指示器

class FOperationHandleRotPlaneRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FOperationHandleRotPlaneRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();
	virtual void BuildPSO();
	virtual int GetRenderLayerType() const { return 11; }

	virtual void ResetPSO();
};