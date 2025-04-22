#include "Rendering.h"
#include "../../Debug/EngineDebug.h"
#include "../../Windows/WindowsEngine.h"
#include "../../Rendering/Engine/DirectX/Core/DirectXRenderingEngine.h"


IRenderingInterface::IRenderingInterface()
{
	//RenderingInterface.push_back(this);
}

IRenderingInterface::~IRenderingInterface()
{/*
	for (vector<IRenderingInterface*>::const_iterator Iter = RenderingInterface.begin();
		Iter != RenderingInterface.end();
		++Iter)
	{
		if (*Iter == this)
		{
			RenderingInterface.erase(Iter);
			break;
		}
	}*/
}

void IRenderingInterface::Init()
{
}

void IRenderingInterface::PreDraw(float DeltaTime)
{
	////重值我们的命令列表
	//ANALYSIS_HRESULT(GetGraphicsCommandList()->Reset(GetCommandAllocator().Get(), NULL));
}

void IRenderingInterface::Draw(float DeltaTime)
{

}

void IRenderingInterface::PostDraw(float DeltaTime)
{
}
