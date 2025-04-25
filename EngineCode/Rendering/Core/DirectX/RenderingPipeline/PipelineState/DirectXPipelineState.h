#pragma once
#include "../../../../../Interface/DirectXDeviceInterfece.h"
#include "../../../RenderingResourcesUpdate.h"
#include "../../../../../Shader/Core/Shader.h"

// 渲染流水线状态
struct FDirectXPipelineState :public IDirectXDeviceInterface_Struct
{
public:
	FDirectXPipelineState();

	void PreDraw(float DeltaTime);
	void Draw(float DeltaTime);
	void PostDraw(float DeltaTime);

	// 重置渲染流水线描述
	void ResetGPSDesc();

	// 绑定输入布局
	void BindInputLayout(const D3D12_INPUT_ELEMENT_DESC* InInputElementDescs, UINT InSize);

	// 绑定根签名
	void BindRootSignature(ID3D12RootSignature* InRootSignature);

	// 绑定shader
	void BindShader(const FShader& InVertexShader, const FShader& InPixelShader);

	//BuildPSO
	void Build();
private:
	// 渲染流水线对象
	ComPtr<ID3D12PipelineState> PSO;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
};
