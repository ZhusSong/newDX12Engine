// 屏幕空间AO
#pragma once
#include "../../../../../../Interface/DirectXDeviceInterface.h"
#include "../../RenderBuffer/NormalBuffer.h"
struct FScreenSpaceAmbientOcclusion :public IDirectXDeviceInterface_Struct
{
	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void Init(int InWidth, int InHeight);

	virtual void Draw(float DeltaTime);
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	void BuildDescriptors();
protected:
	FNormalBuffer NormalBuffer;

};