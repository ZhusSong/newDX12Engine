#include "OutLineEditor.h"
#include "../../EngineCode/Core/World.h"
#include "../../EngineCode/Actor/Core/ActorObject.h"
#include "../../EngineCode/Rendering/Core/DirectX/RenderingPipeline/RenderLayer/RenderLayerManager.h"
#include "../SelectEditor/OperationHandle/MoveArrow.h"


void FOutLineEditor::BuildEditor()
{
}
extern int ActorSelected;

// 添加移动支持
extern GActorObject* SelectedObject;
extern GMoveArrow* MoveArrow;

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

				SelectedObject = Actors[i];

				if (MoveArrow)
				{
					MoveArrow->SetPosition(Actors[i]->GetPosition());
					MoveArrow->SetVisible(true);
				}
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
