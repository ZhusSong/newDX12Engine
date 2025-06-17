#pragma once
#include "../../../../../Interface/DirectXDeviceInterface.h"
#include "../../../RenderingResourcesUpdate.h"
#include "../../../../../Shader/Core/Shader.h"
#include "../RenderingPipelineType.h"

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

	//构建参数
	void BuildParam();

	//BuildPSO
	void Build(int InPSOType);

	// 重置PSO
	void ResetPSO(int InPSOType);

	// 只供不透明层使用
	void ResetPSO();
	// 提供线框渲染模式
	void ResetPSO(bool bWireframe);

	// 设置渲染模式
	void SetFillMode(bool bWireframe);

	// 设置渲染目标
	void SetRenderTarget(int Index, const D3D12_RENDER_TARGET_BLEND_DESC& InRenderTargetBlend);


	void SetRasterizerState(const CD3DX12_RASTERIZER_DESC& InRasterizerDesc);
	void SetDepthStencilState(const CD3DX12_DEPTH_STENCIL_DESC& InDepthStencilDesc);

private:
	//按键捕获
	void CaptureKeyboardKeys();

private:
	// 渲染流水线对象
	unordered_map<int, ComPtr<ID3D12PipelineState>> PSO;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;

	EPipelineState PipelineState;
};
