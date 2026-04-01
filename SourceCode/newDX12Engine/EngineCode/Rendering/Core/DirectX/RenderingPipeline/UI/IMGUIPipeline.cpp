#include "IMGUIPipeline.h"

// 是否使用编辑器引擎
// エディタエンジンを使用するかどうか
#if EDITOR_ENGINE
#include "../../../../../../EditorEngine/EditorEngine.h"
#endif // EDITOR_ENGINE

FIMGUIPipeline::FIMGUIPipeline()
{

}
FIMGUIPipeline::~FIMGUIPipeline()
{
	Exit();
}
void FIMGUIPipeline::Init(ID3D12DescriptorHeap* InHeap, UINT InOffset)
{
	IMGUI_CHECKVERSION();

	// 加载ImGUI配置（Ini）相关的操作
	// ImGUI設定（Ini）関連の操作をロードする
	ImGui::CreateContext();
	bContextCreated = true;
	// 元件的颜色初始化
	// ウィジェットの色を初期化
	ImGui::StyleColorsDark();

	// win32初始化 加载对应的DLL和初始化时间等
	// Win32初期化、対応するDLLのロードや初期化時間など
	ImGui_ImplWin32_Init(GetMainWindowsHandle());

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

	// 初始化后台数据 注册对应的驱动和帧数
	// バックグラウンドデータを初期化し、対応するドライバとフレーム数を登録する
	ImGui_ImplDX12_Init(
		GetD3dDevice().Get(), 1,
		DXGI_FORMAT_R8G8B8A8_UNORM, InHeap,
		CPUDescriptor,
		GPUDescriptor);
	bInitialized = true;
#if EDITOR_ENGINE
	GetEditorEngine()->BuildEditor();
#endif


}

void FIMGUIPipeline::Draw(float DeltaTime)
{
	// 构建PSO(根签名 shader 输入布局) 构建字体
	// PSO（ルートシグネチャ、シェーダー、入力レイアウト）を構築し、フォントを作成
	ImGui_ImplDX12_NewFrame();

	// 初始化 视口 手柄 鼠标移动 帧时间
	// ビューポート、ハンドル、マウス移動、フレーム時間を初期化
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

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
