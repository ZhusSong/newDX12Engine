// 法线缓冲区
// 法線バッファ
#include "Core/RenderBuffer.h"
class FNormalBuffer :public FRenderBuffer
{
	typedef FRenderBuffer Super;

public:
	FNormalBuffer();

	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void Init(int InWidth, int InHeight) override;

	void Draw(float DeltaTime);

	virtual void BuildDescriptors();
	virtual void BuildRenderTargetRTV();
	virtual void BuildSRVDescriptors();
	virtual void BuildRTVDescriptors();

	void SetDepthSRVOffset(UINT InOffset) { DepthSRVOffset = InOffset; }
	UINT GetDepthSRVOffset() const { return DepthSRVOffset; }

	void SetDepthRTVOffset(UINT InOffset) { DepthRTVOffset = InOffset; }
	UINT GetDepthRTVOffset() const { return DepthRTVOffset; }

	std::shared_ptr<FRenderTarget> GetDepthRenderTarget() const { return DepthRenderTarget; }

protected:
	virtual void BuildRenderTargetBuffer(ComPtr<ID3D12Resource>& OutResource);
	void BuildDepthRenderTargetBuffer(ComPtr<ID3D12Resource>& OutResource);

protected:
	UINT DepthSRVOffset;
	UINT DepthRTVOffset;
	DXGI_FORMAT DepthFormat;
	std::shared_ptr<FRenderTarget> DepthRenderTarget;
};
