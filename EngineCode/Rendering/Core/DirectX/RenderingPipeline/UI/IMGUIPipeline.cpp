#include "IMGUIPipeline.h"

FIMGUIPipeline::FIMGUIPipeline()
{

}

void FIMGUIPipeline::Init(ID3D12DescriptorHeap* InHeap, UINT InOffset)
{
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GetMianWindowsHandle());

	UINT CBVDescriptorSize = GetDescriptorHandleIncrementSizeByCBV_SRV_UAV();

	auto CPUDescriptor =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			InHeap->GetCPUDescriptorHandleForHeapStart(),
			InOffset, CBVDescriptorSize);

	auto GPUDescriptor =
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			InHeap->GetGPUDescriptorHandleForHeapStart(),
			InOffset,
			CBVDescriptorSize);

	ImGui_ImplDX12_Init(
		GetD3dDevice().Get(), 1,
		DXGI_FORMAT_R8G8B8A8_UNORM, InHeap,
		CPUDescriptor,
		GPUDescriptor);
}

void FIMGUIPipeline::Draw(float DeltaTime)
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	// 绘制
	Tick(DeltaTime);

	ImGui::Render();

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GetGraphicsCommandList().Get());
}

void FIMGUIPipeline::Exit()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

void FIMGUIPipeline::Tick(float DeltaTime)
{
	bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);//Imgui自带学习案例

	ImGui::Begin("Test");
	//ImGui::Text("asdioakd asd madm askld asd alsd masd .");
	//ImGui::SetWindowPos(ImVec2(0,0),ImGuiCond_Always);
	//ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f));

	//ImGui::Checkbox("bHeight",&bClicked);
	//ImGui::SliderFloat("Hello",&fff,0.f,1.f);
	//ImGui::ColorEdit3("Color",(float*)&CCC);

	ImGui::End();
}
