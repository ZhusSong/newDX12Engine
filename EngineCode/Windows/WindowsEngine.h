#pragma once

#if defined(_WIN32)
#include "../../EngineCode/Core/Engine.h"
#include "../Rendering/Engine/Core/RenderingEngine.h"
class CDirectXRenderingEngine;
class CWorld;

class CWindowsEngine:public CEngine
{
	// 设置DirectX渲染接口类为友元，使其能够访问protected对象
	friend class IDirectXDeviceInterface;
public:
	CWindowsEngine();

	~CWindowsEngine();

	virtual int PreInit(FWinMainCommandParameters InParameters);

	virtual int Init(FWinMainCommandParameters InParameters);
	virtual int PostInit();

	virtual void Tick(float DeltaTime);

	virtual int PreExit();
	virtual int Exit();
	virtual int PostExit();

	CDirectXRenderingEngine* GetRenderingEngine() { return RenderingEngine; }

public:

	bool InitWindows(FWinMainCommandParameters InParameters);

protected:
	//主windows句柄
	HWND MainWindowsHandle;

protected:
	CDirectXRenderingEngine* RenderingEngine;
	CWorld* World;

};
#endif