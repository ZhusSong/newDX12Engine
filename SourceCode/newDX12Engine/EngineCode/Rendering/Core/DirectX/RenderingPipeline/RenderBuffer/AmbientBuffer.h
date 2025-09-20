#pragma once
#include "Core/RenderBuffer.h"

class FAmbientBuffer :public FRenderBuffer
{
	typedef FRenderBuffer Super;

public:
	FAmbientBuffer();

	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void Init(int InWidth, int InHeight) override;

	void Draw(float DeltaTime);

	virtual void BuildDescriptors();
	virtual void BuildRenderTargetRTV();
	virtual void BuildSRVDescriptors();
	virtual void BuildRTVDescriptors();

protected:
	virtual void BuildRenderTargetBuffer(ComPtr<ID3D12Resource>& OutResource);

};