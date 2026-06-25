
#pragma once
#include "../Core/RenderLayer.h"

class FTransparentRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FTransparentRenderLayer();

	virtual void Draw(float DeltaTime);
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);
	virtual void BuildShader();
	virtual void BuildPSO();

	virtual int GetRenderLayerType() const { return 1; }
	virtual void ResetPSO();
	virtual void ResetPSO(EPipelineState InPipelineState);
	virtual void DrawWithPipelineState(float DeltaTime, EPipelineState InPipelineState);

private:
	XMFLOAT4 CachedViewPosition;
};
