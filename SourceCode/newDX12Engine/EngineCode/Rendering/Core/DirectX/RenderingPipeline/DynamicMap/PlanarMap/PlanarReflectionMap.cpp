#include "PlanarReflectionMap.h"
#include "../../Geometry/GeometryMap.h"
#include "../../PipelineState/DirectXPipelineState.h"
#include "../../RenderLayer/RenderLayerManager.h"
#include "../../ConstantBuffer/PlanarReflectionConstantBuffer.h"
#include "../../../../../../Core/Viewport/ClientViewport.h"
#include "../../../../../../Core/World.h"
#include "../../../../../../Core/Camera.h"
#include "../../../../../../Config/EngineRenderConfig.h"
#include "../../../../../../Component/Mesh/Core/MeshComponentType.h"
#include "../../../../../../Component/Mesh/Core/MeshComponent.h"
#include "../../../../../../Mesh/Core/Material/Material.h"
#include <cmath>

FPlanarReflectionMap::FPlanarReflectionMap()
	: Super()
	, ReflectionViewport(nullptr)
{
	CreateRenderTarget<FPlanarMapRenderTarget>();
}

void FPlanarReflectionMap::OnResetSize(int InWidth, int InHeight)
{
	Width = InWidth;
	Height = InHeight;

	if (InWidth > 0 &&
		InHeight > 0 &&
		RenderTarget != nullptr &&
		GeometryMap != nullptr &&
		GeometryMap->GetHeap() != nullptr &&
		RenderTarget->GetCPUSRVOffset().ptr != 0 &&
		RenderTarget->GetCPURenderTargetView().ptr != 0 &&
		DSVDes.ptr != 0)
	{
		Init(InWidth, InHeight);
	}
}

void FPlanarReflectionMap::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
}

void FPlanarReflectionMap::Init(FGeometryMap* InGeometryMap, FDirectXPipelineState* InDirectXPipelineState, FRenderLayerManager* InRenderLayer)
{
	Super::Init(InGeometryMap, InDirectXPipelineState, InRenderLayer);

	if (ReflectionViewport == nullptr)
	{
		FCreateObjectParam Param;
		ReflectionViewport = CreateObject<GClientViewport>(Param, new GClientViewport());
	}
}

void FPlanarReflectionMap::Init(int InWidth, int InHeight)
{
	Super::Init(InWidth, InHeight);

	if (GeometryMap &&
		GeometryMap->GetHeap() &&
		RenderTarget &&
		(RenderTarget->GetCPUSRVOffset().ptr == 0 ||
		 RenderTarget->GetCPURenderTargetView().ptr == 0))
	{
		BuildDescriptors();
	}

	if (RenderTarget)
	{
		RenderTarget->Init(Width, Height, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	BuildDepthStencil();

	if (ReflectionViewport)
	{
		ReflectionViewport->ViewprotInfo = RenderTarget->GetViewport();
		ReflectionViewport->ViewprotRect = RenderTarget->GetScissorRect();
	}
}

void FPlanarReflectionMap::PreDraw(float DeltaTime)
{
	if (!IsExitPlanarReflectionMesh() || ReflectionViewport == nullptr || RenderTarget == nullptr)
	{
		return;
	}

	FPlanarReflectionConstantBuffer DefaultPlanarReflectionConstantBuffer;
	GeometryMap->UpdatePlanarReflectionConstantBuffer(DefaultPlanarReflectionConstantBuffer);

	for (UINT MeshIndex = 0; MeshIndex < GeometryMap->GetPlanarReflectionMeshComponentsSize(); ++MeshIndex)
	{
		CMeshComponent* MirrorMesh = GeometryMap->GetPlanarReflectionMeshComponents((int)MeshIndex);
		if (MirrorMesh == nullptr)
		{
			continue;
		}

		FViewportInfo ReflectionViewportInfo;
		FPlanarReflectionConstantBuffer PlanarReflectionConstantBuffer;
		BuildReflectedViewport(MirrorMesh, ReflectionViewportInfo, PlanarReflectionConstantBuffer);

		UINT ReflectionPassConstantBufferIndex = MeshIndex * 2 + 1;
		UINT MirrorDrawConstantBufferIndex = MeshIndex * 2 + 2;

		FPlanarReflectionConstantBuffer ReflectionPassConstantBuffer = PlanarReflectionConstantBuffer;
		ReflectionPassConstantBuffer.Settings.x = 0.f;
		ReflectionPassConstantBuffer.Settings.z = 1.f;
		ReflectionPassConstantBuffer.Settings.w = 0.001f;
		GeometryMap->UpdatePlanarReflectionConstantBuffer(
			ReflectionPassConstantBuffer,
			ReflectionPassConstantBufferIndex);

		GeometryMap->UpdateCalculationsViewport(
			DeltaTime,
			ReflectionViewportInfo,
			GeometryMap->GetPlanarReflectionViewportOffset());

		CD3DX12_RESOURCE_BARRIER RenderTargetToWrite = CD3DX12_RESOURCE_BARRIER::Transition(
			RenderTarget->GetRenderTarget(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		GetGraphicsCommandList()->ResourceBarrier(1, &RenderTargetToWrite);

		CD3DX12_RESOURCE_BARRIER DepthToWrite = CD3DX12_RESOURCE_BARRIER::Transition(
			DepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_DEPTH_WRITE);
		GetGraphicsCommandList()->ResourceBarrier(1, &DepthToWrite);

		auto RenderTargetViewport = RenderTarget->GetViewport();
		auto RenderTargetScissorRect = RenderTarget->GetScissorRect();
		GetGraphicsCommandList()->RSSetViewports(1, &RenderTargetViewport);
		GetGraphicsCommandList()->RSSetScissorRects(1, &RenderTargetScissorRect);

		const float ClearColor[] = { 0.f, 0.f, 0.f, 1.f };
		GetGraphicsCommandList()->ClearRenderTargetView(
			RenderTarget->GetCPURenderTargetView(),
			ClearColor,
			0,
			nullptr);
		GetGraphicsCommandList()->ClearDepthStencilView(
			DSVDes,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			1.f,
			0,
			0,
			nullptr);

		GetGraphicsCommandList()->OMSetRenderTargets(
			1,
			&RenderTarget->GetCPURenderTargetView(),
			true,
			&DSVDes);

		UINT CBVSize = GeometryMap->GetViewportConstantBufferByteSize();
		auto ViewportAddr = GeometryMap->ViewportGPUVirtualAddress();
		ViewportAddr += GeometryMap->GetPlanarReflectionViewportOffset() * CBVSize;
		GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(1, ViewportAddr);

		GeometryMap->Draw(DeltaTime);
		GeometryMap->DrawPlanarReflectionConstantBuffer(DeltaTime, ReflectionPassConstantBufferIndex);
		RenderLayer->Draw(RENDERLAYER_BACKGROUND, DeltaTime);
		if (std::shared_ptr<FRenderLayer> OpaqueLayer =
			FRenderLayerManager::FindByRenderLayer((int)EMeshRenderLayerType::RENDERLAYER_OPAQUE))
		{
			OpaqueLayer->DrawWithPipelineState(DeltaTime, EPipelineState::PlanarOpaque);
		}
		DrawReflectorObjectsForPlanarPass(DeltaTime, MirrorMesh);
		if (std::shared_ptr<FRenderLayer> TransparentLayer =
			FRenderLayerManager::FindByRenderLayer((int)EMeshRenderLayerType::RENDERLAYER_TRANSPARENT))
		{
			TransparentLayer->UpdateCalculations(DeltaTime, ReflectionViewportInfo);
			TransparentLayer->DrawWithPipelineState(DeltaTime, EPipelineState::PlanarTransparent);
		}

		CD3DX12_RESOURCE_BARRIER RenderTargetToRead = CD3DX12_RESOURCE_BARRIER::Transition(
			RenderTarget->GetRenderTarget(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_GENERIC_READ);
		GetGraphicsCommandList()->ResourceBarrier(1, &RenderTargetToRead);

		CD3DX12_RESOURCE_BARRIER DepthToRead = CD3DX12_RESOURCE_BARRIER::Transition(
			DepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_GENERIC_READ);
		GetGraphicsCommandList()->ResourceBarrier(1, &DepthToRead);

		StartSetMainViewportRenderTarget();
		GeometryMap->Draw(DeltaTime);
		GeometryMap->DrawViewport(DeltaTime);
		GeometryMap->UpdatePlanarReflectionConstantBuffer(PlanarReflectionConstantBuffer, MirrorDrawConstantBufferIndex);
		GeometryMap->DrawPlanarReflectionConstantBuffer(DeltaTime, MirrorDrawConstantBufferIndex);
		RenderLayer->FindObjectDraw(
			DeltaTime,
			RENDERLAYER_OPAQUE_REFLECTOR,
			MirrorMesh);
		GeometryMap->DrawPlanarReflectionConstantBuffer(DeltaTime, 0);
		EndSetMainViewportRenderTarget();
	}
}

void FPlanarReflectionMap::Draw(float DeltaTime)
{
}

bool FPlanarReflectionMap::IsExitPlanarReflectionMesh() const
{
	return GeometryMap && GeometryMap->GetPlanarReflectionMeshComponentsSize() > 0;
}

void FPlanarReflectionMap::BuildDescriptors()
{
	SetSRVOffset(GeometryMap->GetPlanarReflectionTextureIndex());
	SetRTVOffset(
		FEngineRenderConfig::GetRenderConfig()->SwapChainCount +
		6 +
		6);

	BuildSRVOffset();
	BuildRTVOffset();
}

void FPlanarReflectionMap::BuildDepthStencilDescriptor()
{
	UINT DescriptorHandleIncrementSize = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	DSVDes = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		GetDSVHeap()->GetCPUDescriptorHandleForHeapStart(),
		4,
		DescriptorHandleIncrementSize);
}

void FPlanarReflectionMap::BuildDepthStencil()
{
	D3D12_RESOURCE_DESC ResourceDesc = {};
	ResourceDesc.Width = Width;
	ResourceDesc.Height = Height;
	ResourceDesc.Alignment = 0;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_CLEAR_VALUE ClearValue = {};
	ClearValue.DepthStencil.Depth = 1.f;
	ClearValue.DepthStencil.Stencil = 0;
	ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	CD3DX12_HEAP_PROPERTIES Properties(D3D12_HEAP_TYPE_DEFAULT);
	ANALYSIS_HRESULT(GetD3dDevice()->CreateCommittedResource(
		&Properties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&ClearValue,
		IID_PPV_ARGS(DepthStencilBuffer.ReleaseAndGetAddressOf())));

	GetD3dDevice()->CreateDepthStencilView(DepthStencilBuffer.Get(), nullptr, DSVDes);

	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		DepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	GetGraphicsCommandList()->ResourceBarrier(1, &Barrier);

	CD3DX12_RESOURCE_BARRIER BarrierToRead = CD3DX12_RESOURCE_BARRIER::Transition(
		DepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_GENERIC_READ);
	GetGraphicsCommandList()->ResourceBarrier(1, &BarrierToRead);
}

void FPlanarReflectionMap::BuildReflectedViewport(
	CMeshComponent* InMirrorMesh,
	FViewportInfo& OutViewportInfo,
	FPlanarReflectionConstantBuffer& OutPlanarReflectionConstantBuffer)
{
	GCamera* MainCamera = GetWorld()->GetCamera();
	if (MainCamera == nullptr || InMirrorMesh == nullptr)
	{
		return;
	}

	XMFLOAT3 MirrorPosition = InMirrorMesh->GetPosition();
	XMFLOAT3 MirrorNormal = GetMirrorPlaneNormal(InMirrorMesh);

	XMFLOAT3 CameraPosition = MainCamera->GetPosition();
	XMFLOAT3 CameraForward = MainCamera->GetForwardVector();
	XMFLOAT3 CameraUp = MainCamera->GetUPVector();

	XMFLOAT3 MirrorToCamera(
		CameraPosition.x - MirrorPosition.x,
		CameraPosition.y - MirrorPosition.y,
		CameraPosition.z - MirrorPosition.z);
	float CameraPlaneSide =
		MirrorNormal.x * MirrorToCamera.x +
		MirrorNormal.y * MirrorToCamera.y +
		MirrorNormal.z * MirrorToCamera.z;
	float ClipSign = CameraPlaneSide >= 0.f ? 1.f : -1.f;

	XMFLOAT3 ReflectedPosition = ReflectPoint(CameraPosition, MirrorPosition, MirrorNormal);
	XMFLOAT3 ReflectedForward = ReflectVector(CameraForward, MirrorNormal);
	XMFLOAT3 ReflectedUp = ReflectVector(CameraUp, MirrorNormal);

	XMVECTOR ReflectedPositionVector = XMLoadFloat3(&ReflectedPosition);
	XMVECTOR ReflectedForwardVector = XMLoadFloat3(&ReflectedForward);
	XMVECTOR ReflectedUpVector = XMLoadFloat3(&ReflectedUp);
	XMVECTOR ReflectedTargetVector = ReflectedPositionVector + ReflectedForwardVector;

	XMMATRIX ReflectedViewMatrix = XMMatrixLookAtLH(
		ReflectedPositionVector,
		ReflectedTargetVector,
		ReflectedUpVector);

	OutViewportInfo.ViewPosition = XMFLOAT4(
		ReflectedPosition.x,
		ReflectedPosition.y,
		ReflectedPosition.z,
		1.f);
	XMStoreFloat4x4(&OutViewportInfo.ViewMatrix, ReflectedViewMatrix);
	OutViewportInfo.ProjectMatrix = MainCamera->ProjectMatrix;

	XMMATRIX ViewMatrix = XMLoadFloat4x4(&OutViewportInfo.ViewMatrix);
	XMMATRIX ProjectMatrix = XMLoadFloat4x4(&OutViewportInfo.ProjectMatrix);
	XMMATRIX ViewProject = XMMatrixMultiply(ViewMatrix, ProjectMatrix);

	XMMATRIX HalfLambert(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX TexViewProjectionMatrix = XMMatrixMultiply(ViewProject, HalfLambert);
	XMStoreFloat4x4(
		&OutPlanarReflectionConstantBuffer.ReflectionTexViewProjection,
		XMMatrixTranspose(TexViewProjectionMatrix));
	OutPlanarReflectionConstantBuffer.Settings = XMFLOAT4(
		1.f,
		(float)GeometryMap->GetPlanarReflectionTextureIndex(),
		0.f,
		0.f);
	OutPlanarReflectionConstantBuffer.MirrorPlaneNormal = XMFLOAT4(
		MirrorNormal.x,
		MirrorNormal.y,
		MirrorNormal.z,
		ClipSign);
	OutPlanarReflectionConstantBuffer.MirrorPlanePosition = XMFLOAT4(
		MirrorPosition.x,
		MirrorPosition.y,
		MirrorPosition.z,
		1.f);
}

void FPlanarReflectionMap::DrawReflectorObjectsForPlanarPass(float DeltaTime, CMeshComponent* InMirrorMesh)
{
	std::shared_ptr<FRenderLayer> ReflectorLayer =
		FRenderLayerManager::FindByRenderLayer((int)EMeshRenderLayerType::RENDERLAYER_OPAQUE_REFLECTOR);
	if (!ReflectorLayer)
	{
		return;
	}

	ReflectorLayer->ResetPSO(EPipelineState::PlanarReflector);
	for (std::weak_ptr<FRenderingData> WeakRenderingData : ReflectorLayer->GetRenderDatas())
	{
		if (WeakRenderingData.expired())
		{
			continue;
		}

		std::shared_ptr<FRenderingData> RenderingData = WeakRenderingData.lock();
		if (!RenderingData || RenderingData->Mesh == InMirrorMesh)
		{
			continue;
		}

		CMaterial* Material = RenderingData->Mesh->GetMaterialBySlot(RenderingData->MaterialSlotIndex);
		if (Material && Material->IsPlanarReflection())
		{
			continue;
		}

		ReflectorLayer->DrawObject(DeltaTime, WeakRenderingData);
	}
}

XMFLOAT3 FPlanarReflectionMap::ReflectPoint(
	const XMFLOAT3& InPoint,
	const XMFLOAT3& InPlanePoint,
	const XMFLOAT3& InPlaneNormal)
{
	XMVECTOR Point = XMLoadFloat3(&InPoint);
	XMVECTOR PlanePoint = XMLoadFloat3(&InPlanePoint);
	XMVECTOR PlaneNormal = XMVector3Normalize(XMLoadFloat3(&InPlaneNormal));

	XMVECTOR ReflectedPoint =
		Point - 2.0f * XMVector3Dot(Point - PlanePoint, PlaneNormal) * PlaneNormal;

	XMFLOAT3 OutPoint;
	XMStoreFloat3(&OutPoint, ReflectedPoint);
	return OutPoint;
}

XMFLOAT3 FPlanarReflectionMap::ReflectVector(const XMFLOAT3& InVector, const XMFLOAT3& InPlaneNormal)
{
	XMVECTOR Vector = XMLoadFloat3(&InVector);
	XMVECTOR PlaneNormal = XMVector3Normalize(XMLoadFloat3(&InPlaneNormal));

	XMVECTOR ReflectedVector =
		Vector - 2.0f * XMVector3Dot(Vector, PlaneNormal) * PlaneNormal;

	XMFLOAT3 OutVector;
	XMStoreFloat3(&OutVector, XMVector3Normalize(ReflectedVector));
	return OutVector;
}

XMFLOAT3 FPlanarReflectionMap::GetMirrorPlaneNormal(CMeshComponent* InMirrorMesh)
{
	BoundingBox Bounds = InMirrorMesh->GetBoundingBox();
	fvector_3d Scale = InMirrorMesh->GetScale();

	float XSize = std::abs((float)Scale.x) * Bounds.Extents.x;
	float YSize = std::abs((float)Scale.y) * Bounds.Extents.y;
	float ZSize = std::abs((float)Scale.z) * Bounds.Extents.z;

	XMFLOAT3 MirrorNormal = InMirrorMesh->GetForwardVector();
	if (XSize <= YSize && XSize <= ZSize)
	{
		MirrorNormal = InMirrorMesh->GetRightVector();
	}
	else if (YSize <= XSize && YSize <= ZSize)
	{
		MirrorNormal = InMirrorMesh->GetUPVector();
	}

	XMVECTOR MirrorNormalVector = XMVector3Normalize(XMLoadFloat3(&MirrorNormal));
	XMStoreFloat3(&MirrorNormal, MirrorNormalVector);
	return MirrorNormal;
}
