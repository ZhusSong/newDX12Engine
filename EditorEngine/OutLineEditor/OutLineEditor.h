// 25.7.15 李
#pragma once
#include "../Core/EditorBase.h"
#include "../../EngineCode/Interface/DirectXDeviceInterface.h"

class GActorObject;
class FOutLineEditor :public FEditorBase, public IDirectXDeviceInterface
{
public:
	virtual void BuildEditor();
	virtual void DrawEditor(float DeltaTime);
	virtual void ExitEditor();
	void HighlightDisplayObject(GActorObject* InObject);
};