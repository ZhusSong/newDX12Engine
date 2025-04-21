#include "MeshManager.h"
#include "../../Config/EngineRenderConfig.h"
#include "../BoxMesh.h"
#include "../ConeMesh.h"
#include "../CustomMesh.h"
#include "../CylinderMesh.h"
#include "../SphereMesh.h"
#include "../PlaneMesh.h"
#include "ObjectTransform.h"
#include "../../Rendering/Core/RenderingResourcesUpdate.h"
#include "../../Rendering/Engine/DirectX/Core/DirectXRenderingEngine.h"
#include "../../Rendering/Core/Buffer/ConstructBuffer.h"
#include "../../Math/EngineMath.h"
#include "../../Core/Viewport/ViewportTransformation.h"

CMeshManage::CMeshManage()
    :VertexSizeInBytes(0)
    , VertexStrideInBytes(0)
    , IndexSizeInBytes(0)
    , IndexFormat(DXGI_FORMAT_R16_UINT)
    , IndexSize(0)
    , WorldMatrix(FObjectTransform::IdentityMatrix4x4())
{

}

void CMeshManage::Init()
{
   
}

void CMeshManage::BuildMesh(const FMeshRenderingData* InRenderingData)
{
    //构建CBV栈描述
    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;
    HeapDesc.NumDescriptors = 2;
    HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    HeapDesc.NodeMask = 0;
    GetD3dDevice()->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&CBVHeap));

    
    //常量缓冲区的构建
    //////////////////////////////
    UINT DescriptorOffset = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    ObjectConstants = make_shared<FRenderingResourcesUpdate>();
    ObjectConstants->Init(GetD3dDevice().Get(), sizeof(FObjectTransform), 1);

    D3D12_GPU_VIRTUAL_ADDRESS Addr = ObjectConstants->GetBuffer()->GetGPUVirtualAddress();

    D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
    CBVDesc.BufferLocation = Addr;
    CBVDesc.SizeInBytes = ObjectConstants->GetConstantBufferByteSize();

    CD3DX12_CPU_DESCRIPTOR_HANDLE DesHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CBVHeap->GetCPUDescriptorHandleForHeapStart());
    DesHandle.Offset(0, DescriptorOffset);

    GetD3dDevice()->CreateConstantBufferView(
        &CBVDesc,
        DesHandle);

    ViewportConstants = make_shared<FRenderingResourcesUpdate>();
    ViewportConstants->Init(GetD3dDevice().Get(), sizeof(FViewportTransformation), 1);

    D3D12_GPU_VIRTUAL_ADDRESS ViewPortAddr = ViewportConstants->GetBuffer()->GetGPUVirtualAddress();

    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewportCBVDesc;
    ViewportCBVDesc.BufferLocation = ViewPortAddr;
    ViewportCBVDesc.SizeInBytes = ViewportConstants->GetConstantBufferByteSize();

    DesHandle.Offset(1, DescriptorOffset);

    GetD3dDevice()->CreateConstantBufferView(
        &ViewportCBVDesc,
        DesHandle);

    //构建根签名
    CD3DX12_ROOT_PARAMETER RootParam[2];

    //CBV描述表
    CD3DX12_DESCRIPTOR_RANGE DescriptorRangeObjCBV;
    DescriptorRangeObjCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE DescriptorRangeViewportCBV;
    DescriptorRangeViewportCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

    RootParam[0].InitAsDescriptorTable(1, &DescriptorRangeObjCBV);
    RootParam[1].InitAsDescriptorTable(1, &DescriptorRangeViewportCBV);

    CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(
        2,
        RootParam,
        0,
        nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    //ｴｴｽｨ
    ComPtr<ID3DBlob> SerializeRootSignature;
    ComPtr<ID3DBlob> ErrorBlob;
    D3D12SerializeRootSignature(
        &RootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        SerializeRootSignature.GetAddressOf(),
        ErrorBlob.GetAddressOf());

    if (ErrorBlob)
    {
        Engine_Log_Error("%s", (char*)ErrorBlob->GetBufferPointer());
    }

    //ｴｴｽｨ
    GetD3dDevice()->CreateRootSignature(
        0,
        SerializeRootSignature->GetBufferPointer(),
        SerializeRootSignature->GetBufferSize(),
        IID_PPV_ARGS(&RootSignature));

    //构建Shader
    //HLSL
    VertexShader.BuildShaders(L"../newDX12Engine/Shader/VertexShader.hlsl", "VertexShaderMain", "vs_5_0");
    PixelShader.BuildShaders(L"../newDX12Engine/Shader/VertexShader.hlsl", "PixelShaderMain", "ps_5_0");

    InputElementDesc =
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };

    //构建模型
    //////////////////////////
    VertexStrideInBytes = sizeof(FVertex);
    IndexSize = InRenderingData->IndexData.size();

    //ｻ｡ﾁﾋﾄ｣ﾐﾍﾊｾﾝｵﾄｴ｡
    VertexSizeInBytes = InRenderingData->VertexData.size() * VertexStrideInBytes;
    IndexSizeInBytes = IndexSize * sizeof(uint16_t);

    ANALYSIS_HRESULT(D3DCreateBlob(VertexSizeInBytes, &CPUVertexBufferPtr));
    memcpy(CPUVertexBufferPtr->GetBufferPointer(), InRenderingData->VertexData.data(), VertexSizeInBytes);

    ANALYSIS_HRESULT(D3DCreateBlob(IndexSizeInBytes, &CPUIndexBufferPtr));
    memcpy(CPUIndexBufferPtr->GetBufferPointer(), InRenderingData->IndexData.data(), IndexSizeInBytes);

    ConstructBuffer::FConstructBuffer ConstructBuffer;
    GPUVertexBufferPtr = ConstructBuffer.ConstructDefaultBuffer(
        VertexBufferTmpPtr,
        InRenderingData->VertexData.data(), VertexSizeInBytes);

    GPUIndexBufferPtr = ConstructBuffer.ConstructDefaultBuffer(IndexBufferTmpPtr,
        InRenderingData->IndexData.data(), IndexSizeInBytes);

    //PSO ﾁｮﾏﾟｰｨ
    D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
    memset(&GPSDesc, 0, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

    //ｰｨﾊ菠・ｼｾﾖ
    GPSDesc.InputLayout.pInputElementDescs = InputElementDesc.data();
    GPSDesc.InputLayout.NumElements = (UINT)InputElementDesc.size();

    //ｰｨｸｩﾃ・
    GPSDesc.pRootSignature = RootSignature.Get();

    //ｰｨｶ･ｵ聆ﾅﾉｫﾆ惲・
    GPSDesc.VS.pShaderBytecode = reinterpret_cast<BYTE*>(VertexShader.GetBufferPointer());
    GPSDesc.VS.BytecodeLength = VertexShader.GetBufferSize();

    //ｰｨﾏﾘﾗﾅﾉｫﾆ・
    GPSDesc.PS.pShaderBytecode = PixelShader.GetBufferPointer();
    GPSDesc.PS.BytecodeLength = PixelShader.GetBufferSize();

    //ﾅ葷ﾃｹ籃､ｻｯﾗｴﾌｬ
    GPSDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;//ﾒﾔﾏﾟｿｽﾊｽﾏﾔﾊｾ

    //0000..0000
    GPSDesc.SampleMask = UINT_MAX;

    GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    GPSDesc.NumRenderTargets = 1;

    GPSDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    GPSDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    GPSDesc.SampleDesc.Count = GetEngine()->GetRenderingEngine()->GetDXGISampleCount();
    GPSDesc.SampleDesc.Quality = GetEngine()->GetRenderingEngine()->GetDXGISampleQuality();

    //RTV ｺﾍ DSVｸｽ
    GPSDesc.RTVFormats[0] = GetEngine()->GetRenderingEngine()->GetBackBufferFormat();
    GPSDesc.DSVFormat = GetEngine()->GetRenderingEngine()->GetDepthStencilFormat();

    ANALYSIS_HRESULT(GetD3dDevice()->CreateGraphicsPipelineState(&GPSDesc, IID_PPV_ARGS(&PSO)))
}


void CMeshManage::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
    //XMUINT3 MeshPos = XMUINT3(5.0f, 5.0f, 5.0f);

    //XMVECTOR Pos = XMVectorSet(MeshPos.x, MeshPos.y, MeshPos.z, 1.0f);
    //XMVECTOR ViewTarget = XMVectorZero();
    //XMVECTOR ViewUp = XMVectorSet(0.f, 1.0f, 0.f, 0.f);

    //XMMATRIX ViewLookAt = XMMatrixLookAtLH(Pos, ViewTarget, ViewUp);
    //XMStoreFloat4x4(const_cast<XMFLOAT4X4*>(&ViewportInfo.ViewMatrix), ViewLookAt);

    XMMATRIX ViewMatrix = XMLoadFloat4x4(&ViewportInfo.ViewMatrix);
    XMMATRIX ProjectMatrix = XMLoadFloat4x4(&ViewportInfo.ProjectMatrix);
    XMMATRIX ATRIXWorld = XMLoadFloat4x4(&WorldMatrix);

    //  XMMATRIX WVP = ATRIXWorld * ViewMatrix * ATRIXProject;

    FObjectTransform ObjectTransformation;
    XMStoreFloat4x4(&ObjectTransformation.World, XMMatrixTranspose(ATRIXWorld));
    ObjectConstants->Update(0, &ObjectTransformation);

    //ｸ・ﾂﾊﾓｿﾚ
    XMMATRIX ViewProject = XMMatrixMultiply(ViewMatrix, ProjectMatrix);
    FViewportTransformation ViewportTransformation;
    XMStoreFloat4x4(&ViewportTransformation.ViewProjectionMatrix, XMMatrixTranspose(ViewProject));

    ViewportConstants->Update(0, &ViewportTransformation);
}

void CMeshManage::PostDraw(float DeltaTime)
{

}

void CMeshManage::Draw(float DeltaTime)
{
    ID3D12DescriptorHeap* DescriptorHeap[] = { CBVHeap.Get() };
    GetGraphicsCommandList()->SetDescriptorHeaps(_countof(DescriptorHeap), DescriptorHeap);

    GetGraphicsCommandList()->SetGraphicsRootSignature(RootSignature.Get());

    D3D12_VERTEX_BUFFER_VIEW VBV = GetVertexBufferView();

    //ｰｨ葷ﾈｾﾁｮﾏﾟﾉﾏｵﾄﾊ菠・ﾛ｣ｬｿﾉﾒﾔﾔﾚﾊ菠・ｰﾅ菷ﾗｶﾎｴｫﾈ・･ｵ飜ｾﾝ
    GetGraphicsCommandList()->IASetVertexBuffers(
        0,//ﾆｼﾊ菠・ﾛ 0-15 
        1,//k k+1 ... k+n-1 
        &VBV);

    D3D12_INDEX_BUFFER_VIEW IBV = GetIndexBufferView();
    GetGraphicsCommandList()->IASetIndexBuffer(&IBV);

    //ｶｨﾒ衾ﾒﾃﾇﾒｪｻ贍ﾆｵﾄﾄﾄﾖﾖﾍｼﾔｪ ｵ・ﾏﾟ ﾃ・
    GetGraphicsCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT DescriptorOffset = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    auto DesHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CBVHeap->GetGPUDescriptorHandleForHeapStart());

    DesHandle.Offset(0, DescriptorOffset);
    GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(0, DesHandle);

    DesHandle.Offset(1, DescriptorOffset);
    GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(1, DesHandle);

    //ﾕ贏ｵﾄｻ贍ﾆ
    GetGraphicsCommandList()->DrawIndexedInstanced(
        IndexSize,//ｶ･ｵ飜ﾁｿ
        1,//ｻ贍ﾆﾊｵﾀﾊﾁｿ
        0,//ｶ･ｵ羹ｺｳ衂ﾚﾒｻｸｻｻ贍ﾆｵﾄﾋ
        0,//GPU ｴﾓﾋｻｺｳ衂ﾁﾈ｡ｵﾄｵﾚﾒｻｸｵﾄﾎｻﾖﾃ｡｣
        0);//ﾔﾚｴﾓｶ･ｵ羹ｺｳ衂ﾁﾈ｡ﾃｿｸｵﾀﾊｾﾝﾖｮﾇｰﾌ晴ﾓｵｽﾃｿｸｵﾄﾖｵ｡｣
}

void CMeshManage::PreDraw(float DeltaTime)
{
    GetGraphicsCommandList()->Reset(GetCommandAllocator().Get(), PSO.Get());
}

D3D12_VERTEX_BUFFER_VIEW CMeshManage::GetVertexBufferView()
{
    D3D12_VERTEX_BUFFER_VIEW VBV;
    VBV.BufferLocation = GPUVertexBufferPtr->GetGPUVirtualAddress();
    VBV.SizeInBytes = VertexSizeInBytes;
    VBV.StrideInBytes = VertexStrideInBytes;

    return VBV;
}

D3D12_INDEX_BUFFER_VIEW CMeshManage::GetIndexBufferView()
{
    D3D12_INDEX_BUFFER_VIEW IBV;
    IBV.BufferLocation = GPUIndexBufferPtr->GetGPUVirtualAddress();
    IBV.SizeInBytes = IndexSizeInBytes;
    IBV.Format = IndexFormat;

    return IBV;
}

CMesh* CMeshManage::CreateSphereMesh(float InRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
    return CreateMesh<CSphereMesh>(InRadius, InAxialSubdivision, InHeightSubdivision);
}

CMesh* CMeshManage::CreateMesh(string& InPath)
{
    return CreateMesh<CCustomMesh>(InPath);
}

CMesh* CMeshManage::CreateBoxMesh(float InHeight, float InWidth, float InDepth)
{
    return CreateMesh<CBoxMesh>(InHeight, InWidth, InDepth);
}

CMesh* CMeshManage::CreateConeMesh(float InRadius, float InHeight, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
    return CreateMesh<CConeMesh>(InRadius, InHeight, InAxialSubdivision, InHeightSubdivision);
}

CMesh* CMeshManage::CreateCylinderMesh(float InTopRadius, float InBottomRadius, float InHeight, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
    return CreateMesh<CCylinderMesh>(InTopRadius, InBottomRadius, InHeight, InAxialSubdivision, InHeightSubdivision);
}

CMesh* CMeshManage::CreatePlaneMesh(float InHeight, float InWidth, uint32_t InHeightSubdivide, uint32_t InWidthSubdivide)
{
    return CreateMesh<CPlaneMesh>(InHeight, InWidth, InHeightSubdivide, InWidthSubdivide);
}

template<class T, typename ...ParamTypes>
T* CMeshManage::CreateMesh(ParamTypes && ...Params)
{
    T* MyMesh = new T();

    //ﾌ睚｡ﾄ｣ﾐﾍﾗﾊﾔｴ
    FMeshRenderingData MeshData;
    MyMesh->CreateMesh(MeshData, forward<ParamTypes>(Params)...);

    MyMesh->BeginInit();

    //ｹｹｽｨmesh
    BuildMesh(&MeshData);

    MyMesh->Init();

    return MyMesh;
}