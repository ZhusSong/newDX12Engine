#include "EngineMinimal.h"
#include "EngineFactory.h"
#include "Debug/Log/SimpleLog.h"

int Init(CEngine* InEngine, HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(_WIN32)
	FWinMainCommandParameters WinMainParameters(hInstance, prevInstance, cmdLine, showCmd);
#endif 

	int ReturnValue = InEngine->PreInit(
#if defined(_WIN32)
		WinMainParameters
#endif 
	);

	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine pre initialization error, check and initialization problem.", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->Init(
#if defined(_WIN32)
		WinMainParameters
#endif 
	);
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine initialization error, please check the initialization problem.", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->PostInit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine post initialization error, please check the initialization problem.", ReturnValue);
		return ReturnValue;
	}

	return ReturnValue;
}

void Tick(CEngine* InEngine)
{
	float DeltaTime = 0.03f;
	InEngine->Tick(DeltaTime);

	//Sleep(DeltaTime);
}

int Exit(CEngine* InEngine)
{
	int ReturnValue = InEngine->PreExit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine pre exit failed.", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->Exit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine exit failed.", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->PostExit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine post exit failed.", ReturnValue);
		return ReturnValue;
	}


	return ReturnValue;
}


CEngine* Engine = NULL;
//hInstance 当前实例       現在のインスタンス
//prevInstance 上次的实例  前回のインスタンス
//cmdLine 传递命令         コマンドライン
//showCmd 多少条cmd        cmd状態
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	int ReturnValue = 0;

	Engine = FEngineFactory::CreateEngine();

	if (Engine)
	{
		// 初始化
		// 初期化
		Init(Engine, hInstance, prevInstance, cmdLine, showCmd);

		MSG EngineMsg = { 0 };

		// 渲染出图
		// レンダリング出力
		while (EngineMsg.message != WM_QUIT)
		{
			//PM_NOREMOVE 消息不从队列里除掉。                           メッセージをキューから削除しない。
			//PM_REMOVE   消息从队列里除掉。                             メッセージをキューから削除する。
			//PM_NOYIELD  此标志使系统不释放等待调用程序空闲的线程       このフラグは、呼び出し元がアイドル状態になるのを待っているスレッドをシステムが解放しないようにする。
			// 
			//PM_QS_INPUT 处理鼠标和键盘消息。                           マウスとキーボードのメッセージを処理する。
			//PM_QS_PAINT 处理画图消息。                                 描画メッセージを処理する。
			//PM_QS_POSTMESSAGE 处理所有被寄送的消息，包括计时器和热键。 タイマーやホットキーを含む、ポストされたすべてのメッセージを処理する。
			//PM_QS_SENDMESSAGE 处理所有发送消息。                       送信されたすべてのメッセージを処理する。
			if (PeekMessage(&EngineMsg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&EngineMsg);
				DispatchMessage(&EngineMsg);
			}
			else
			{
				Tick(Engine);
			}
		}

		ReturnValue = Exit(Engine);
		delete Engine;
		Engine = NULL;
	}
	else
	{
		ReturnValue = 1;
	}

	Engine_Log("[%i]The engine has exited.", ReturnValue);
	return ReturnValue;
}