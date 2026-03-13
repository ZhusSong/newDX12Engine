#pragma once

#if defined(_WIN32)
#include "../../EngineCode/Core/Engine.h"
#include "../Rendering/Engine/Core/RenderingEngine.h"

class CDirectXRenderingEngine;
class CWorld;
class CMeshManager;

class CWindowsEngine:public CEngine
{
	// 设置DirectX渲染接口类为友元，使其能够访问protected对象
	// DirectXレンダリングインターフェースクラスをフレンドに設定し、protectedオブジェクトにアクセスできるようにする
	friend class IDirectXDeviceInterface;
public:
	CWindowsEngine();

	~CWindowsEngine();

	virtual int PreInit(FWinMainCommandParameters InParameters);

	virtual int Init(FWinMainCommandParameters InParameters);
	virtual int PostInit();

	virtual void Tick(float DeltaTime);
	virtual void OnResetSize(int InWidth, int InHeight);

	virtual int PreExit();
	virtual int Exit();
	virtual int PostExit();

	CDirectXRenderingEngine* GetRenderingEngine() const { return RenderingEngine; }
public:
	// 初始化项目路径
	// プロジェクトパスを初期化
	void InitPath();

public:
	CMeshManager* GetMeshManager()const;
	CWorld* GetWorld() const { return World; }
public:

	bool InitWindows(FWinMainCommandParameters InParameters);

protected:
	// 主windows句柄
	// メインwindowsハンドル
	HWND MainWindowsHandle;

protected:
	CDirectXRenderingEngine* RenderingEngine;

	// 添加编辑器引擎
	// エディターエンジンを追加
#if EDITOR_ENGINE
	class CEditorEngine* EditorEngine;
#endif

	CWorld* World;

};
#endif