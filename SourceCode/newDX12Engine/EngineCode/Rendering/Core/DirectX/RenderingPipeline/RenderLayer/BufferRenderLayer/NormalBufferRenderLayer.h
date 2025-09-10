#include "../Core/RenderLayer.h"

class FNormalBufferRenderLayer :public FRenderLayer
{
	typedef FRenderLayer Super;
public:
	FNormalBufferRenderLayer();

	virtual void Draw(float DeltaTime);

	virtual void BuildShader();
	virtual void BuildPSO();
	virtual int GetRenderLayerType() const { return 20; }

	virtual void ResetPSO();
};