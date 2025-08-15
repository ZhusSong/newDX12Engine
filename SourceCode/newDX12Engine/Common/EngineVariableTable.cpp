#include "EngineVariableTable.h"
#include "../EngineCode/EngineMinimal.h"

int ActorSelected = 0;
class GActorObject* SelectedObject = nullptr;
class CMeshComponent* SelectAxisComponent = nullptr;
bool bOperationHandleSelect = false;

#if EDITOR_ENGINE
class GMoveArrow* MoveArrow = nullptr;
#endif // EDITOR_ENGINE