#include "RaycastSystemLibrary.h"
#include "../Core/Camera.h"
#include "../Config/EngineRenderConfig.h"
#include "../Core/World.h"
#include "../Collision/CollisionSceneQuery.h"

bool FRaycastSystemLibrary::HitResultByScreen(CWorld* InWorld, int ScreenX, int ScreenY, FCollisionResult& OutResult)
{
	if (GCamera* InCamera = InWorld->GetCamera())
	{
		int H = FEngineRenderConfig::GetRenderConfig()->ScrrenHight;
		int W = FEngineRenderConfig::GetRenderConfig()->ScrrenWidth;

		fvector_2d View;
		View.x = (2.f * ScreenX / W - 1.f) / InCamera->ProjectMatrix._11;
		View.y = (-2.f * ScreenY / H + 1.f) / InCamera->ProjectMatrix._22;

		// 视口下的 原点 和 方向
		XMVECTOR OriginPoint = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		XMVECTOR Direction = XMVectorSet(View.x, View.y, 1.f, 0.f);

		XMMATRIX ViewMatrix = XMLoadFloat4x4(&InCamera->ViewMatrix);
		XMVECTOR ViewMatrixDeterminant = XMMatrixDeterminant(ViewMatrix);
		XMMATRIX ViewInverseMatrix = XMMatrixInverse(&ViewMatrixDeterminant, ViewMatrix);

		FCollisionSceneQuery::RaycastSingle(InWorld, OriginPoint, Direction, ViewInverseMatrix, OutResult);
	}

	return false;
}
