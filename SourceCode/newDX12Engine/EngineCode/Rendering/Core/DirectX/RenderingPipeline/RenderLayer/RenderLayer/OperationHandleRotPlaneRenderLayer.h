#pragma once
#include "../Core/RenderLayer.h"

// 平面旋转指示器
// 平面回転インジケーター

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