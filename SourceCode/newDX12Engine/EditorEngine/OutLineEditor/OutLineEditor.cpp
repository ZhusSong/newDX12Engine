#include "OutLineEditor.h"
#include "../../EngineCode/Core/World.h"
#include "../../EngineCode/Actor/Core/ActorObject.h"
#include "../../EngineCode/Rendering/Core/DirectX/RenderingPipeline/RenderLayer/RenderLayerManager.h"

#include "../../Common/OperationHandleSelectManager.h"

extern int ActorSelected;

void FOutLineEditor::BuildEditor()
{
}
// 设置高亮的文本
// テキストのハイライト
void FOutLineEditor::DrawEditor(float DeltaTime)
{
	ImGui::Begin("OutLine");
	if (CWorld* InWorld = GetWorld())
	{
		ImGui::BeginChild("OutLine pane", ImVec2(150, 0), true);

		const vector<GActorObject*> Actors = InWorld->GetActors();
		for (int i = 0; i < Actors.size(); i++)
		{
			char ObjectNameString[128] = { 0 };
			sprintf(ObjectNameString, "%s", Actors[i]->GetName().c_str());

			if (ImGui::Selectable(ObjectNameString, ActorSelected == i))
			{
				HighlightDisplayObject(Actors[i]);

				// 设置选择对象
				// 選択オブジェクトを設定する
				FOperationHandleSelectManager::Get()->SetNewSelectedObject(Actors[i]);

				// 显示操作手柄
				// 操作ハンドルを表示する
				FOperationHandleSelectManager::Get()->DisplaySelectedOperationHandle();
			}
		}

		ImGui::EndChild();
	}
	ImGui::End();
}

void FOutLineEditor::ExitEditor()
{
}

void FOutLineEditor::HighlightDisplayObject(GActorObject* InObject)
{
	if (FRenderLayerManager* InLayer = GetRenderLayerManager())
	{
		InLayer->HighlightDisplayObject(InObject);
	}
}