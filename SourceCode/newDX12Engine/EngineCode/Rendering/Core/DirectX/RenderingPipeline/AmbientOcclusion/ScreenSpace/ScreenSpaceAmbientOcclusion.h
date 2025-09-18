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
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	void DrawViewConstantBufferViews(float DeltaTime, const FViewportInfo& ViewportInfo);

	void BuildDescriptors();

	virtual void Build();

	void BindBuildPSO();

	void BuildPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC& OutPSODesc);

	void BuildSSAOViewConstantBuffer();

	void SaveToSSAOBuffer();

	void BuildDepthBuffer();
protected:
	FSSAODirectXRootSignature DirectXRootSignature;

	FNormalBuffer NormalBuffer;

	FAmbientBuffer AmbientBuffer;

	FSampleVolumeBuffer SampleVolumeBuffer;

	FNoiseBuffer NoiseBuffer;

	// SSAO常量缓冲区视图
	FConstantBufferViews SSAOViewConstantBufferViews;


protected:
	FRenderLayerManager* RenderLayer;
	FGeometryMap* GeometryMap;

};