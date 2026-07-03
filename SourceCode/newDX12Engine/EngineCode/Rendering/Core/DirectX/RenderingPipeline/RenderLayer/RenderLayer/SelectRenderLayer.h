
#pragma once
#include "../Core/RenderLayer.h"

class FSelectRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FSelectRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();
	virtual void BuildPSO();
	virtual int GetRenderLayerType() const { return 9; }
	virtual bool ShouldDrawInCurrentLayer(const FRenderingData& InRenderingData, const CMaterial* InMaterial) const override;
};
