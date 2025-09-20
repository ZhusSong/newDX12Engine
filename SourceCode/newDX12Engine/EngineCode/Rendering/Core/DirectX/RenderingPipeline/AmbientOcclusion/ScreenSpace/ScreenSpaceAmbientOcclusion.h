// 屏幕空间AO
#pragma once
#include "../../../../../../Interface/DirectXDeviceInterface.h"
#include "../../RenderBuffer/NormalBuffer.h"
#include "../../RenderBuffer/AmbientBuffer.h"
#include "../../RenderBuffer/SampleVolumeBuffer.h"
#include "../../RenderBuffer/NoiseBuffer.h"
#include "../../RootSignature/SSAODirectXRootSignature.h"
#include "../../ConstantBuffer/ConstantBufferViews.h"


struct FScreenSpaceAmbientOcclusion :public IDirectXDeviceInterface_Struct
{
public:
	FScreenSpaceAmbientOcclusion();
	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void Init(int InWidth, int InHeight);

	virtual void Draw(float DeltaTime);
	virtual void DrawSSAO(float DeltaTime);
	virtual void DrawBilateralBlur(float DeltaTime, UINT InDrawNum = 1);
	virtual void DrawResources(float DeltaTime);
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

public:
	void DrawViewConstantBufferViews(float DeltaTime, const FViewportInfo& ViewportInfo);
	void DrawBlurConstantBufferViews(float DeltaTime, const FViewportInfo& ViewportInfo);

	void BuildDescriptors();

	virtual void Build();

	void BindBuildPSO();

	void BuildPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC& OutPSODesc);

	void SaveToSSAOBuffer();

	void BuildDepthBuffer();
	void BuildBlurWeights(float InSigma, bool bReBuildBlurWeights = false);

protected:	
	//渲染双边模糊
	virtual void DrawBlur(float DeltaTime, bool bHorizontal);
	virtual void DrawBlurHorizontal(float DeltaTime);
	virtual void DrawBlurVertical(float DeltaTime);

protected:
	ID3D12Resource* GetDrawResources(bool bHorizontal);
	CD3DX12_GPU_DESCRIPTOR_HANDLE* GetDrawSRVResources(bool bHorizontal);
	CD3DX12_CPU_DESCRIPTOR_HANDLE* GetDrawRTVResources(bool bHorizontal);

	void SetRoot32BitConstant(bool bHorizontal);
public:
	void BuildSSAOViewConstantBuffer();
	void BuildSSAOBlurParamConstantBuffer();

public:
	UINT GetDepthBufferSRVOffset() const;

	UINT GetNormalBufferSRVOffset() const;
	UINT GetNormalBufferRTVOffset() const;

	UINT GetNoiseBufferSRVOffset() const;

	UINT GetAmbientBufferSRVOffset() const;
	UINT GetAmbientBufferRTVOffset() const;

	UINT GetBilateralBlurSRVOffset() const;
	UINT GetBilateralBlurRTVOffset() const;
	//void BuildSSAOViewConstantBuffer();

	//void SaveToSSAOBuffer();

protected:
	FSSAODirectXRootSignature DirectXRootSignature;

	FNormalBuffer NormalBuffer;

	FAmbientBuffer AmbientBuffer;
	FAmbientBuffer BilateralBlur;

	FSampleVolumeBuffer SampleVolumeBuffer;

	FNoiseBuffer NoiseBuffer;

	// SSAO常量缓冲区视图
	FConstantBufferViews SSAOViewConstantBufferViews;
	FConstantBufferViews SSAOBlurConstantBufferParam;


protected:
	FRenderLayerManager* RenderLayer;
	FGeometryMap* GeometryMap;


protected:
	std::vector<float> BlurWeights;
	float BlurRadius;

};