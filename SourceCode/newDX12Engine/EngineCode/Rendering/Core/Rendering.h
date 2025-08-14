#pragma once
#include "../../Core/CoreObject/GuidInterface.h"
#include "../../Core/Engine.h"
#if defined(_WIN32)
#include "../../Windows/WindowsEngine.h"
#else
#endif

// 提供渲染内容的接口
class IRenderingInterface 
{
	friend class CDirectXRenderingEngine;
public:
	IRenderingInterface();
	virtual ~IRenderingInterface();

	virtual void Init();

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);
};
