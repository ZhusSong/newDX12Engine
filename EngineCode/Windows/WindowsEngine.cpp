#include "WindowsEngine.h"
#include "../Debug/EngineDebug.h"
#include "../Config//EngineRenderConfig.h"
#include "../Rendering/Core/Rendering.h"
#include "../Mesh/BoxMesh.h"
#include "../Mesh/SphereMesh.h"
#include "../Mesh/CylinderMesh.h"
#include "../Mesh/ConeMesh.h"
#include "../Mesh/PlaneMesh.h"
#include "../Mesh/CustomMesh.h"
#include "../Core/CoreObject/CoreMinimalObject.h"
#include "../Core/World.h"
#include "../Core/Camera.h"

#include "../Rendering/Engine/DirectX/DirectX12RenderingEngine.h"

#if defined(_WIN32)
#include "WindowsMessageProcessing.h"

CWindowsEngine::CWindowsEngine()
	:RenderingEngine(new CDirectX12RenderingEngine())
{

}
CWindowsEngine::~CWindowsEngine()
{
	delete RenderingEngine;
}

int CWindowsEngine::PreInit(FWinMainCommandParameters InParameters)
{
	//日志系统初始化
	const char LogPath[] = "log";
	init_log_system(LogPath);
	Engine_Log("Log Init.");

	//处理命令


	Engine_Log("Engine pre initialization complete.");

	//渲染引擎初始化
	RenderingEngine->PreInit(InParameters);

	return 0;
}

int CWindowsEngine::Init(FWinMainCommandParameters InParameters)
{
	InitWindows(InParameters);

	RenderingEngine->SetMainWindowsHandle(MainWindowsHandle);

	RenderingEngine->Init(InParameters);

	World = CreateObject<CWorld>(new CWorld());

	Engine_Log("Engine initialization complete.");

	return 0;
}

int CWindowsEngine::PostInit()
{
	Engine_Log("Engine post initialization complete.");

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
			ViewportInfo.ViewMatrix = World->GetCamera()->ViewMatrix;
			ViewportInfo.ProjectMatrix = World->GetCamera()->ProjectMatrix;
			RenderingEngine->UpdateCalculations(DeltaTime, ViewportInfo);

			RenderingEngine->Tick(DeltaTime);
		}
	}
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

bool CWindowsEngine::InitWindows(FWinMainCommandParameters InParameters)
{
	//注册窗口
	WNDCLASSEX WindowsClass;
	WindowsClass.cbSize = sizeof(WNDCLASSEX);//该对象实际占用多大内存
	WindowsClass.cbClsExtra = 0;//是否需要额外空间
	WindowsClass.cbWndExtra = 0;//是否需要额外内存
	WindowsClass.hbrBackground = nullptr;//如果有设置哪就是GDI擦除
	WindowsClass.hCursor = LoadCursor(NULL, IDC_ARROW);//设置一个箭头光标
	WindowsClass.hIcon = nullptr; //应用程序放在磁盘上显示的图标
	WindowsClass.hIconSm = NULL;//应用程序显示在左上角的图标
	WindowsClass.hInstance = InParameters.HInstance; //窗口实例
	WindowsClass.lpszClassName = L"DX12Engine";//窗口名字
	WindowsClass.lpszMenuName = nullptr;//
	WindowsClass.style = CS_VREDRAW | CS_HREDRAW;//怎么绘制窗口 垂直和水平重绘
	WindowsClass.lpfnWndProc = EngineWindowProc;//消息处理函数

	//注册窗口
	ATOM RegisterAtom = RegisterClassEx(&WindowsClass);
	if (!RegisterAtom)
	{
		Engine_Log_Error("Register windows class fail.");
		MessageBox(NULL, L"Register windows class fail.", L"Error", MB_OK);
	}

	RECT Rect = { 0,0,FEngineRenderConfig::GetRenderConfig()->ScrrenWidth,FEngineRenderConfig::GetRenderConfig()->ScrrenHight };

	//@rect 适口
	//WS_OVERLAPPEDWINDOW 适口风格
	//NULL 没有菜单
	AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, NULL);

	int WindowWidth = Rect.right - Rect.left;
	int WindowHight = Rect.bottom - Rect.top;

	MainWindowsHandle = CreateWindowEx(
		NULL,//窗口额外的风格
		L"DX12Engine", // 窗口名称
		L"DX12 Engine",//会显示在窗口的标题栏上去
		WS_OVERLAPPEDWINDOW, //窗口风格
		WINDOWS_LOCATION_X, WINDOWS_LOCATION_Y,//窗口的坐标
		WindowWidth, WindowHight,//
		NULL, //副窗口句柄
		nullptr, //菜单句柄
		InParameters.HInstance,//窗口实例
		NULL);//
	if (!MainWindowsHandle)
	{
		Engine_Log_Error("CreateWindow Failed..");
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	//显示窗口
	ShowWindow(MainWindowsHandle, SW_SHOW);

	//窗口是脏的，刷新一下
	UpdateWindow(MainWindowsHandle);

	Engine_Log("InitWindows complete.");

	return true;
}

#endif