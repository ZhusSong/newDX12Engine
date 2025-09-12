#include "SSAODirectXRootSignature.h"

FSSAODirectXRootSignature::FSSAODirectXRootSignature()
{

}

void FSSAODirectXRootSignature::BuildRootSignature(UINT InTextureNum)
{
    CD3DX12_ROOT_PARAMETER RootParam[3];

    CD3DX12_DESCRIPTOR_RANGE DescriptorNormalTextureSRV;
    DescriptorNormalTextureSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE DescriptorDepthTextureSRV;
    DescriptorDepthTextureSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

    RootParam[0].InitAsConstantBufferView(0);

    // ShadowMap 法线贴图
    RootParam[1].InitAsDescriptorTable(1, &DescriptorNormalTextureSRV, D3D12_SHADER_VISIBILITY_PIXEL);

    // ShadowMap 深度贴图
    RootParam[2].InitAsDescriptorTable(1, &DescriptorDepthTextureSRV, D3D12_SHADER_VISIBILITY_PIXEL);

    StaticSamplerObject.BuildStaticSampler();

    CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(
        3,
        RootParam,
        StaticSamplerObject.GetSize(),//采样数量
        StaticSamplerObject.GetData(),//采样PTR
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // 序列化
    ComPtr<ID3DBlob> SerializeRootSignature;
    ComPtr<ID3DBlob> ErrorBlob;
    D3D12SerializeRootSignature(
        &RootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        SerializeRootSignature.GetAddressOf(),
        ErrorBlob.GetAddressOf());

    if (ErrorBlob)
    {
        char* p = (char*)ErrorBlob->GetBufferPointer();
        Engine_Log_Error("%s", p);
    }

    // 创建
    GetD3dDevice()->CreateRootSignature(
        0,
        SerializeRootSignature->GetBufferPointer(),
        SerializeRootSignature->GetBufferSize(),
        IID_PPV_ARGS(&RootSignature));
}

void FSSAODirectXRootSignature::PreDraw(float DeltaTime)
{
    Super::PreDraw(DeltaTime);
}

void FSSAODirectXRootSignature::Draw(float DeltaTime)
{
}

void FSSAODirectXRootSignature::PostDraw(float DeltaTime)
{
}
