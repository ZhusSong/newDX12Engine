#pragma once
#include "../Core/DynamicMap.h"
#include "../../RenderTarget/PlanarMapRenderTarget.h"

class GClientViewport;
class CMeshComponent;
struct FPlanarReflectionConstantBuffer;

struct FViewportInfo;
struct FGeometryMap;
struct FDirectXPipelineState;

class FPlanarReflectionMap : public FDynamicMap
{
	typedef FDynamicMap Super;

public:
	FPlanarReflectionMap();

	virtual void OnResetSize(int InWidth, int InHeight);
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);
	virtual void Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer);
	virtual void Init(int InWidth, int InHeight);
	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);

	bool IsExitPlanarReflectionMesh() const;
	void BuildDescriptors();
	void BuildDepthStencilDescriptor();
	void BuildDepthStencil();

protected:
	void BuildReflectedViewport(
		CMeshComponent* InMirrorMesh,
		FViewportInfo& OutViewportInfo,
		FPlanarReflectionConstantBuffer& OutPlanarReflectionConstantBuffer);
	void DrawReflectorObjectsForPlanarPass(float DeltaTime, CMeshComponent* InMirrorMesh);
	static XMFLOAT3 ReflectPoint(const XMFLOAT3& InPoint, const XMFLOAT3& InPlanePoint, const XMFLOAT3& InPlaneNormal);
	static XMFLOAT3 ReflectVector(const XMFLOAT3& InVector, const XMFLOAT3& InPlaneNormal);
	static XMFLOAT3 GetMirrorPlaneNormal(CMeshComponent* InMirrorMesh);

protected:
	GClientViewport* ReflectionViewport;
	ComPtr<ID3D12Resource> DepthStencilBuffer;
	CD3DX12_CPU_DESCRIPTOR_HANDLE DSVDes;
};
