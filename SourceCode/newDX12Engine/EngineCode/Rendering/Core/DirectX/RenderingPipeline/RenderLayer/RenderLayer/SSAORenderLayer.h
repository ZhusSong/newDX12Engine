#pragma once
#include "../Core/RenderLayer.h"
// SSAO渲染层
// SSAO描画レイヤー
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