#include "RaycastSystemLibrary.h"
#include "../Core/Camera.h"
#include "../Config/EngineRenderConfig.h"
#include "../Core/World.h"
#include "../Collision/CollisionSceneQuery.h"


bool FRaycastSystemLibrary::GetRaycastByScreenParam(
	CWorld* InWorld,
	const fvector_2id& ScreenXY,
	XMVECTOR& OriginPoint,
	XMVECTOR& Direction,
	XMMATRIX& ViewInverseMatrix)
{
	if (GCamera* InCamera = InWorld->GetCamera())
	{
		int H = InCamera->GetHeight();
		int W = InCamera->GetWidth();

		// 将像素坐标转换到[-1,1]的标准设备坐标系(NDC)
		// ピクセル座標を[-1,1]の正規化デバイス座標系（NDC）に変換
		fvector_2d View;
		View.x = (2.f * ScreenXY.x / W - 1.f) / InCamera->ProjectMatrix._11;
		View.y = (-2.f * ScreenXY.y / H + 1.f) / InCamera->ProjectMatrix._22;

		// 构建视图空间射线
		// ビュー空間上のレイを構築
		OriginPoint = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		Direction = XMVectorSet(View.x, View.y, 1.f, 0.f);

		// 计算视图逆矩阵
		// ビュー行列の逆行列を計算
		XMMATRIX ViewMatrix = XMLoadFloat4x4(&InCamera->ViewMatrix);
		XMVECTOR ViewMatrixDeterminant = XMMatrixDeterminant(ViewMatrix);
		ViewInverseMatrix = XMMatrixInverse(&ViewMatrixDeterminant, ViewMatrix);

		return true;
	}

	return false;
}
bool FRaycastSystemLibrary::HitResultByScreen(CWorld* InWorld, int ScreenX, int ScreenY, FCollisionResult& OutResult)
{
	XMVECTOR OriginPoint;
	XMVECTOR Direction;
	XMMATRIX ViewInverseMatrix;
	if (GetRaycastByScreenParam(
		InWorld,
		fvector_2id(ScreenX, ScreenY),
		OriginPoint,
		Direction,
		ViewInverseMatrix))
	{
		return FCollisionSceneQuery::RaycastSingle(InWorld, OriginPoint, Direction, ViewInverseMatrix, OutResult);
	}

	return false;
}

bool FRaycastSystemLibrary::HitSpecificObjectsResultByScreen(
	CWorld* InWorld,
	GActorObject* InSpecificObjects,
	const std::vector<CComponent*>& IgnoreComponents,
	int ScreenX, int ScreenY, 
	FCollisionResult& OutResult)
{
	XMVECTOR OriginPoint;
	XMVECTOR Direction;
	XMMATRIX ViewInverseMatrix;
	if (GetRaycastByScreenParam(
		InWorld,
		fvector_2id(ScreenX, ScreenY),
		OriginPoint,
		Direction,
		ViewInverseMatrix))
	{
		return FCollisionSceneQuery::RaycastSingle(
			InWorld,
			InSpecificObjects,
			IgnoreComponents,
			OriginPoint, Direction, ViewInverseMatrix, OutResult);
	}

	return false;
}
