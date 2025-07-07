#include "IMGUIPipeline.h"

// 是否使用编辑器引擎
#if EDITOR_ENGINE
#include "../../../../../../EditorEngine/EditorEngine.h"
#endif // EDITOR_ENGINE

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

#if EDITOR_ENGINE
	GetEditorEngine()->BuildEditor();
#endif


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
#if EDITOR_ENGINE
	GetEditorEngine()->ExitEditor();
#endif

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

void FIMGUIPipeline::Tick(float DeltaTime)
{
#if EDITOR_ENGINE
	GetEditorEngine()->DrawEditor(DeltaTime);
#endif
}
