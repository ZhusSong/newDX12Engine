#pragma once

#if defined(_WIN32)
#include "../../EngineCode/Core/Engine.h"
#include "../Rendering/Engine/Core/RenderingEngine.h"
#include "../Rendering/Engine/DirectX/Core/DirectXRenderingEngine.h"

class CWindowsEngine:public CEngine
{
	friend class IRenderingInterface;
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
	HWND MainWindowsHandle;//主windows句柄
	CDirectXRenderingEngine* RenderingEngine;

};
#endif