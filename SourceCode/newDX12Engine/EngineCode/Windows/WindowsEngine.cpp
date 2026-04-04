#include "WindowsEngine.h"
#include "CoreObject/CoreMinimalObject.h"
#include <Windows.h>

#include "../Debug/EngineDebug.h"
#include "../Config//EngineRenderConfig.h"
#include "../Rendering/Core/Rendering.h"
#include "../Mesh/BoxMesh.h"
#include "../Mesh/SphereMesh.h"
#include "../Mesh/CylinderMesh.h"
#include "../Mesh/ConeMesh.h"
#include "../Mesh/PlaneMesh.h"
#include "../Mesh/CustomMesh.h"
#include "../Core/World.h"
#include "../Core/Camera.h"
#include "../Mesh//Core//MeshManager.h"

#include "../Rendering/Engine/DirectX/DirectX12RenderingEngine.h"

#include "../Component/Input/Input.h"

#include "../Core/Construction/MacroConstruction.h"

#if EDITOR_ENGINE
#include "../../EditorEngine/EditorEngine.h"
#endif


#if defined(_WIN32)
#include "WindowsMessageProcessing.h"

CWindowsEngine::CWindowsEngine()
	:RenderingEngine(new CDirectX12RenderingEngine())
#if EDITOR_ENGINE
	, EditorEngine(new CEditorEngine())
#endif 
{

}
CWindowsEngine::~CWindowsEngine()
{
	delete RenderingEngine;
}

int CWindowsEngine::PreInit(FWinMainCommandParameters InParameters)
{
	// 自适应屏幕大小绑定
	// 画面サイズに適応したバインド
	OnResetSizeDelegate.AddFunction(this, &CWindowsEngine::OnResetSize);


	InitPath();

	// 日志系统初始化
	// ログシステムの初期化
	std::string LogPath =
		FEnginePathHelper::RelativeToAbsolutePath(
			FEnginePathHelper::GetEngineLogsPath());

	init_log_system(LogPath.c_str());
	Engine_Log("Log Init.");



	Engine_Log("Engine pre initialization complete.");

	// 渲染引擎初始化
	// レンダリングエンジンの初期化
	RenderingEngine->PreInit(InParameters);

	return 0;
}

int CWindowsEngine::Init(FWinMainCommandParameters InParameters)
{
	InitWindows(InParameters);

	RenderingEngine->SetMainWindowsHandle(MainWindowsHandle);

	RenderingEngine->Init(InParameters);

	// 注册对应的world对象
	// 対応するワールドオブジェクトを登録
	BUILD_OBJECT_PARAMETERS_BY_NO_COMPONENT(, this);
	World = CreateObject<CWorld>(Param, new CWorld());
	RenderingEngine->World = World;

	Engine_Log("Engine initialization complete.");

	return 0;
}

int CWindowsEngine::PostInit()
{
	Engine_Log("Engine post initialization complete.");

	// 先初始化渲染引擎
	// まずレンダリングエンジンを初期化
	RenderingEngine->PostInit();

	for (auto& Tmp : GObjects)
	{
		Tmp->BeginInit();
	}

	return 0;
}

void CWindowsEngine::Tick(float DeltaTime)
{
	for (auto& Tmp : GObjects)
	{
		if (Tmp->IsTick())
		{
			Tmp->Tick(DeltaTime);
		}
	}
	if (World)
	{
		if (World->GetCamera())
		{
			FViewportInfo ViewportInfo;

			XMFLOAT3 ViewPosition = World->GetCamera()->GetPosition();
			ViewportInfo.ViewPosition = XMFLOAT4(ViewPosition.x, ViewPosition.y, ViewPosition.z, 1.f);

			ViewportInfo.ViewMatrix = World->GetCamera()->ViewMatrix;

			ViewportInfo.ProjectMatrix = World->GetCamera()->ProjectMatrix;

			RenderingEngine->UpdateCalculations(DeltaTime, ViewportInfo);

			RenderingEngine->Tick(DeltaTime);
		}
	}
}

void CWindowsEngine::OnResetSize(int InWidth, int InHeight)
{
	RenderingEngine->OnResetSize(InWidth, InHeight);

#if EDITOR_ENGINE
	EditorEngine->OnResetSize(InWidth, InHeight);
#endif
}

int CWindowsEngine::PreExit()
{
	RenderingEngine->PreExit();

	Engine_Log("Engine post exit complete.");
	return 0;
}

int CWindowsEngine::Exit()
{
	RenderingEngine->Exit();

	Engine_Log("Engine exit complete.");
	return 0;
}

int CWindowsEngine::PostExit()
{
	FEngineRenderConfig::Destroy();
	RenderingEngine->PostExit();

	Engine_Log("Engine post exit complete.");
	return 0;
}

void CWindowsEngine::InitPath()
{
	auto CreateFileDirectory = [](const std::string& InPath)
		{
			std::string LogPath =
				FEnginePathHelper::RelativeToAbsolutePath(
					InPath);
			// 创建路径
			// パスを作成
			create_file_directory(LogPath.c_str());
		};

	// 日志系统初始化
	// ログシステムの初期化
	CreateFileDirectory(FEnginePathHelper::GetEngineLogsPath());

	CreateFileDirectory(FEnginePathHelper::GetEngineAssetPath());

	wstring ShadersPathW = FEnginePathHelper::GetEngineShadersPath();
	char ShaderPaht[1024] = { 0 };
	wchar_t_to_char(ShaderPaht, 1024, ShadersPathW.c_str());
	CreateFileDirectory(ShaderPaht);

	CreateFileDirectory(FEnginePathHelper::GetEngineBinariesPath());
}

CMeshManager* CWindowsEngine::GetMeshManager()const
{
	return RenderingEngine->GetMeshManager();
}


bool CWindowsEngine::InitWindows(FWinMainCommandParameters InParameters)
{
	// Avoid OS bitmap upscaling on high-DPI monitors.
	using SetProcessDpiAwarenessContextFunc = BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT);
	if (HMODULE User32Module = GetModuleHandleW(L"user32.dll"))
	{
		if (SetProcessDpiAwarenessContextFunc SetDpiAwarenessContext =
			reinterpret_cast<SetProcessDpiAwarenessContextFunc>(
				GetProcAddress(User32Module, "SetProcessDpiAwarenessContext")))
		{
			SetDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		}
		else
		{
			SetProcessDPIAware();
		}
	}

	// 注册窗口
	// ウィンドウを登録
	WNDCLASSEX WindowsClass;
	WindowsClass.cbSize = sizeof(WNDCLASSEX);//该对象实际占用多大内存            // このオブジェクトが実際に占有するメモリサイズ
	WindowsClass.cbClsExtra = 0;//是否需要额外空间                               // 追加スペースが必要かどうか
	WindowsClass.cbWndExtra = 0;//是否需要额外内存                               // 追加メモリが必要かどうか
	WindowsClass.hbrBackground = nullptr;//如果有设置哪就是GDI擦除               // 設定されている場合はGDI消去 
	WindowsClass.hCursor = LoadCursor(NULL, IDC_ARROW);//设置一个箭头光标        // 矢印カーソルを設定
	WindowsClass.hIcon = nullptr; //应用程序放在磁盘上显示的图标                 // アプリケーションがディスク上に表示するアイコン
	WindowsClass.hIconSm = NULL;//应用程序显示在左上角的图标                     // アプリケーションが左上隅に表示するアイコン
	WindowsClass.hInstance = InParameters.HInstance; //窗口实例                  // ウィンドウインスタンス
	WindowsClass.lpszClassName = L"DX12Engine";//窗口名字                        // ウィンドウ名
	WindowsClass.lpszMenuName = nullptr;//
	WindowsClass.style = CS_VREDRAW | CS_HREDRAW;//怎么绘制窗口 垂直和水平重绘   // ウィンドウの描画方法 垂直および水平方向の再描画
	WindowsClass.lpfnWndProc = EngineWindowProc;//消息处理函数                   // メッセージ処理関数

	// 注册窗口
	// ウィンドウを登録
	ATOM RegisterAtom = RegisterClassEx(&WindowsClass);
	if (!RegisterAtom)
	{
		Engine_Log_Error("Register windows class fail.");
		MessageBox(NULL, L"Register windows class fail.", L"Error", MB_OK);
	}

	RECT Rect = { 0,0,FEngineRenderConfig::GetRenderConfig()->ScreenWidth,FEngineRenderConfig::GetRenderConfig()->ScreenHight };

	//@rect 
	//WS_OVERLAPPEDWINDOW 
	//NULL 
	AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, NULL);

	int WindowWidth = Rect.right - Rect.left;
	int WindowHight = Rect.bottom - Rect.top;

	MainWindowsHandle = CreateWindowEx(
		NULL,//窗口额外的风格                                  // ウィンドウの追加スタイル
		L"DX12Engine", // 窗口名称                             // ウィンドウ名
		L"DX12 Engine",//会显示在窗口的标题栏上去              // ウィンドウのタイトルバーに表示される
		WS_OVERLAPPEDWINDOW, //窗口风格                        // ウィンドウスタイル
		WINDOWS_LOCATION_X, WINDOWS_LOCATION_Y,//窗口的坐标    // ウィンドウスタイル
		WindowWidth, WindowHight,//
		NULL, //副窗口句柄                                     // 子ウィンドウハンドル
		nullptr, //菜单句柄                                    // メニューハンドル
		InParameters.HInstance,//窗口实例                      // ウィンドウインスタンス
		NULL);//
	if (!MainWindowsHandle)
	{
		Engine_Log_Error("CreateWindow Failed..");
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	// 显示窗口
	// ウィンドウを表示
	ShowWindow(MainWindowsHandle, SW_SHOW);

	UpdateWindow(MainWindowsHandle);

	Engine_Log("InitWindows complete.");

	return true;
}

#endif
