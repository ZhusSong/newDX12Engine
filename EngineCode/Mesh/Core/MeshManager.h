// 2025.4.李
#pragma once
#include "../../Core/CoreObject/CoreMinimalObject.h"
#include "../../Shader/Core/Shader.h"
#include "MeshType.h"
#include "Mesh.h"
#include "../../Interface/DirectXDeviceInterface.h"
#include "../../Core/Viewport/ViewportInfo.h"
#include "../../Rendering/Core/DirectX/RenderingPipeline/RenderingPipeline.h"

class FRenderingResourcesUpdate;
class CMeshComponent;

class CMeshManager :public CCoreMinimalObject, 
				   public IRenderingInterface,
				   public IDirectXDeviceInterface
{
public:
	CMeshManager();

	virtual void Init();

	virtual void BuildMesh();
	virtual void UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo);

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);

	/*D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();*/
public:
	CMeshComponent* CreateBoxMeshComponent(
		float InHeight,
		float InWidth,
		float InDepth);

	CMeshComponent* CreateConeMeshComponent(
		float InRadius,
		float InHeight,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);

	CMeshComponent* CreateCylinderMeshComponent(
		float InTopRadius,
		float InBottomRadius,
		float InHeight,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);

	CMeshComponent* CreatePlaneMeshComponent(
		float InHeight,
		float InWidth,
		uint32_t InHeightSubdivide,
		uint32_t InWidthSubdivide);

	CMeshComponent* CreateSphereMeshComponent(
		float InRadius,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);

	CMeshComponent* CreateMeshComponent(string& InPath);

protected:
	template<class T, typename ...ParamTypes>
	T* CreateMeshComponent(ParamTypes &&...Params);

protected:
	FRenderingPipeline RenderingPipeline;


	/*ComPtr<ID3DBlob> CPUVertexBufferPtr;
	ComPtr<ID3DBlob> CPUIndexBufferPtr;

	ComPtr<ID3D12Resource> GPUVertexBufferPtr;
	ComPtr<ID3D12Resource> GPUIndexBufferPtr;

	ComPtr<ID3D12Resource> VertexBufferTmpPtr;
	ComPtr<ID3D12Resource> IndexBufferTmpPtr;

	ComPtr<ID3D12RootSignature>  RootSignature;
	ComPtr<ID3D12DescriptorHeap> CBVHeap;

	shared_ptr<FRenderingResourcesUpdate> ObjectConstants;
	shared_ptr<FRenderingResourcesUpdate> ViewportConstants;*/
//
//	
//protected:
//	UINT VertexSizeInBytes;
//	UINT VertexStrideInBytes;
//
//	UINT IndexSizeInBytes;
//	DXGI_FORMAT IndexFormat;
//	UINT IndexSize;
//
//	XMFLOAT4X4 WorldMatrix;
};
