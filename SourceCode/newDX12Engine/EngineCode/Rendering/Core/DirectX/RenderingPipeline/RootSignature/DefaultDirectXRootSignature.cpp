#include "DefaultDirectXRootSignature.h"

FDefaultDirectXRootSignature::FDefaultDirectXRootSignature()
{

}

// 构建根签名
// ルートシグネチャを構築する
void FDefaultDirectXRootSignature::BuildRootSignature(UINT InTextureNum)
{   
    CD3DX12_ROOT_PARAMETER RootParam[11];

    // texture描述表(包括cubemap)
    // テクスチャ記述テーブル（キューブマップを含む）
    CD3DX12_DESCRIPTOR_RANGE DescriptorRangeTextureSRV;
    DescriptorRangeTextureSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        InTextureNum, 4);

    // SSAO
    CD3DX12_DESCRIPTOR_RANGE DescriptorSSAOMapSRV;
    DescriptorSSAOMapSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);


    // 静态CubeMap
    // 静的キューブマップ
    CD3DX12_DESCRIPTOR_RANGE DescriptorRangeCubeMapSRV;
    DescriptorRangeCubeMapSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    //ShadowMap
    CD3DX12_DESCRIPTOR_RANGE DescriptorShadowMapSRV;
    DescriptorShadowMapSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

    //ShadowCubeMap
    CD3DX12_DESCRIPTOR_RANGE DescriptorShadowCubeMapSRV;
    DescriptorShadowCubeMapSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

    RootParam[0].InitAsConstantBufferView(0);//对象  // オブジェクト
    RootParam[1].InitAsConstantBufferView(1);//视口  // ビューポート
    RootParam[2].InitAsConstantBufferView(2);//灯光  // ライト
    RootParam[3].InitAsConstantBufferView(3);//雾    // フォグ

    // 材质
    // マテリアル
    RootParam[4].InitAsShaderResourceView(0, 1);
    // 2D贴图
    // 2Dテクスチャ
    RootParam[5].InitAsDescriptorTable(1, &DescriptorRangeTextureSRV, D3D12_SHADER_VISIBILITY_PIXEL);
    // CubeMap贴图
    // キューブマップテクスチャ
    RootParam[6].InitAsDescriptorTable(1, &DescriptorRangeCubeMapSRV, D3D12_SHADER_VISIBILITY_PIXEL);

    //ShadowMap
    RootParam[7].InitAsDescriptorTable(1, &DescriptorShadowMapSRV, D3D12_SHADER_VISIBILITY_PIXEL);

    //ShadowMap
    RootParam[8].InitAsDescriptorTable(1, &DescriptorShadowCubeMapSRV, D3D12_SHADER_VISIBILITY_PIXEL);

    //SSAO
    RootParam[9].InitAsDescriptorTable(1, &DescriptorSSAOMapSRV, D3D12_SHADER_VISIBILITY_PIXEL);

    // Planar reflection constants
    // 平面反射定数
    RootParam[10].InitAsConstantBufferView(4);

    // 构建静态采样
    // 静的サンプラーを構築する
    StaticSamplerObject.BuildStaticSampler();

    CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(
        11,
        RootParam,
        StaticSamplerObject.GetSize(),//采样数量  // サンプル数
        StaticSamplerObject.GetData(),//采样PTR   // サンプラーPTR
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // 序列化
    //  シリアライズ
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

    GetD3dDevice()->CreateRootSignature(
        0,
        SerializeRootSignature->GetBufferPointer(),
        SerializeRootSignature->GetBufferSize(),
        IID_PPV_ARGS(&RootSignature));
}

void FDefaultDirectXRootSignature::PreDraw(float DeltaTime)
{
    Super::PreDraw(DeltaTime);
}

void FDefaultDirectXRootSignature::Draw(float DeltaTime)
{
}

void FDefaultDirectXRootSignature::PostDraw(float DeltaTime)
{
}
