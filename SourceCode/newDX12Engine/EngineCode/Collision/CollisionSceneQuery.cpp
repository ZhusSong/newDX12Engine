#include "CollisionSceneQuery.h"
#include "../Rendering/Core/DirectX/RenderingPipeline/Geometry/GeometryMap.h"
#include "../Core/World.h"
#include "../Component/Mesh/Core/MeshComponent.h"
#include "../Actor/Core/ActorObject.h"
#include "../Core/Camera.h"

namespace
{
	float GetWorldHitDistance(
		const std::shared_ptr<FRenderingData>& InRenderingData,
		const XMVECTOR& LocalOriginPoint,
		const XMVECTOR& LocalDirection,
		float LocalHitTime,
		const XMVECTOR& WorldOriginPoint)
	{
		const XMVECTOR LocalHitPoint = XMVectorMultiplyAdd(
			XMVectorReplicate(LocalHitTime),
			LocalDirection,
			LocalOriginPoint);

		const XMMATRIX WorldMatrix = XMLoadFloat4x4(&InRenderingData->WorldMatrix);
		const XMVECTOR WorldHitPoint = XMVector3TransformCoord(LocalHitPoint, WorldMatrix);

		return XMVectorGetX(XMVector3Length(WorldHitPoint - WorldOriginPoint));
	}
}

void GetRaycastDataByLocal(
	std::shared_ptr<FRenderingData>& InRenderingData,
	const XMVECTOR& OriginPoint,
	const XMVECTOR& Direction,
	const XMMATRIX& ViewInverseMatrix,
	XMVECTOR& OutLocalOriginPoint,
	XMVECTOR& OutLocalDirection)
{

	// 转模型局部
	// モデルのローカル空間へ変換する
	XMMATRIX WorldMatrix = XMLoadFloat4x4(&InRenderingData->WorldMatrix);
	XMVECTOR WorldMatrixDeterminant = XMMatrixDeterminant(WorldMatrix);
	XMMATRIX WorldMatrixInverse = XMMatrixInverse(&WorldMatrixDeterminant, WorldMatrix);

	// 局部矩阵
	// ローカル行列
	XMMATRIX LocalMatrix = XMMatrixMultiply(ViewInverseMatrix, WorldMatrixInverse);

	// 局部空间的射线点位置
	// ローカル空間におけるレイの原点位置
	OutLocalOriginPoint = XMVector3TransformCoord(OriginPoint, LocalMatrix);
	OutLocalDirection = XMVector3TransformNormal(Direction, LocalMatrix);

	// 单位化
	// 正規化
	OutLocalDirection = XMVector3Normalize(OutLocalDirection);
}

bool FCollisionSceneQuery::IsIgnoreComponents(CComponent* InComponent, const std::vector<CComponent*>& IgnoreComponents)
{
	for (auto& Tmp : IgnoreComponents)
	{
		if (Tmp == InComponent)
		{
			return true;
		}
	}

	return false;
}

bool FCollisionSceneQuery::RaycastSingle(
    CWorld* InWorld, 
    const XMVECTOR& OriginPoint,
    const XMVECTOR& Direction, 
    const XMMATRIX& ViewInverseMatrix,
    FCollisionResult& OutResult)
{
	float FinalTime = FLT_MAX;
	XMVECTOR WorldOriginPoint = XMVector3TransformCoord(OriginPoint, ViewInverseMatrix);
	for (size_t i = 0; i < FGeometry::RenderingDatas.size(); i++)
	{
		std::shared_ptr<FRenderingData>& InRenderingData = FGeometry::RenderingDatas[i];

		if (InRenderingData->Mesh->IsPickup())
		{
			XMVECTOR LocalOriginPoint;
			XMVECTOR LocalDirection;

			// 转为局部坐标
			// ローカル座標へ変換する
			GetRaycastDataByLocal(
				InRenderingData,
				OriginPoint,
				Direction,
				ViewInverseMatrix,
				LocalOriginPoint,
				LocalDirection);

			// 射线是否可以和AABB相交
			// レイがAABBと交差するかどうか
			float BoundTime = 0.f;
			if (InRenderingData->Bounds.Intersects(LocalOriginPoint, LocalDirection, BoundTime))
			{
				if (BoundTime > 0.f)
				{
					const float BoundWorldDistance = GetWorldHitDistance(
						InRenderingData,
						LocalOriginPoint,
						LocalDirection,
						BoundTime,
						WorldOriginPoint);

					if (BoundWorldDistance < FinalTime)
					{
						if (InRenderingData->MeshRenderingData)
						{
							UINT TriangleNumber = InRenderingData->IndexSize / 3;

							float TriangleTime = FLT_MAX;
							for (UINT i = 0; i < TriangleNumber; i++)
							{
								fvector_3id Indices;
								Indices.x = InRenderingData->MeshRenderingData->IndexData[InRenderingData->IndexOffsetPosition + i * 3 + 0];
								Indices.y = InRenderingData->MeshRenderingData->IndexData[InRenderingData->IndexOffsetPosition + i * 3 + 1];
								Indices.z = InRenderingData->MeshRenderingData->IndexData[InRenderingData->IndexOffsetPosition + i * 3 + 2];

								XMVECTOR Vertex0 = XMLoadFloat3(&InRenderingData->MeshRenderingData->VertexData[InRenderingData->VertexOffsetPosition + Indices.x].Position);
								XMVECTOR Vertex1 = XMLoadFloat3(&InRenderingData->MeshRenderingData->VertexData[InRenderingData->VertexOffsetPosition + Indices.y].Position);
								XMVECTOR Vertex2 = XMLoadFloat3(&InRenderingData->MeshRenderingData->VertexData[InRenderingData->VertexOffsetPosition + Indices.z].Position);

								float TriangleTestsTime = 0.f;
								if (TriangleTests::Intersects(LocalOriginPoint, LocalDirection, Vertex0, Vertex1, Vertex2, TriangleTestsTime))
								{
									const float WorldHitDistance = GetWorldHitDistance(
										InRenderingData,
										LocalOriginPoint,
										LocalDirection,
										TriangleTestsTime,
										WorldOriginPoint);

									if (WorldHitDistance < TriangleTime)
									{
										TriangleTime = WorldHitDistance;
									}
								}
							}

							if (TriangleTime < FinalTime)
							{
								FinalTime = TriangleTime;

								OutResult.bHit = true;
								OutResult.Component = InRenderingData->Mesh;
								OutResult.Time = TriangleTime;
								if (InRenderingData->Mesh)
								{
									OutResult.Actor = dynamic_cast<GActorObject*>(InRenderingData->Mesh->GetOuter());
								}

								// 拿到渲染数据
								// レンダリングデータを取得する
								OutResult.RenderingData = InRenderingData;
							}
						}
					}
				}
			}
		}
	}

	return OutResult.bHit;
}



bool FCollisionSceneQuery::RaycastSingle(
	CWorld* InWorld,
	GActorObject* InSpecificObjects,
	const std::vector<CComponent*>& IgnoreComponents,
	const XMVECTOR& OriginPoint,
	const XMVECTOR& Direction,
	const XMMATRIX& ViewInverseMatrix,
	FCollisionResult& OutResult)
{
	float FinalTime = FLT_MAX;
	XMVECTOR WorldOriginPoint = XMVector3TransformCoord(OriginPoint, ViewInverseMatrix);
	for (size_t i = 0; i < FGeometry::RenderingDatas.size(); i++)
	{
		std::shared_ptr<FRenderingData>& InRenderingData = FGeometry::RenderingDatas[i];

		if (InRenderingData->Mesh->IsPickup())
		{
			if (!IsIgnoreComponents(InRenderingData->Mesh, IgnoreComponents))
			{
				XMVECTOR LocalOriginPoint;
				XMVECTOR LocalDirection;

				GetRaycastDataByLocal(
					InRenderingData,
					OriginPoint,
					Direction,
					ViewInverseMatrix,
					LocalOriginPoint,
					LocalDirection);

				float BoundTime = 0.f;
				if (InRenderingData->Bounds.Intersects(LocalOriginPoint, LocalDirection, BoundTime))
				{
					if (GActorObject* InActorObject = dynamic_cast<GActorObject*>(InRenderingData->Mesh->GetOuter()))
					{
						if (InActorObject == InSpecificObjects)
						{
							const float WorldHitDistance = GetWorldHitDistance(
								InRenderingData,
								LocalOriginPoint,
								LocalDirection,
								BoundTime,
								WorldOriginPoint);

							if (WorldHitDistance < FinalTime)
							{
								FinalTime = WorldHitDistance;

								OutResult.bHit = true;
								OutResult.Component = InRenderingData->Mesh;
								OutResult.Time = WorldHitDistance;
								OutResult.Actor = InActorObject;

								// 拿到渲染数据
								// レンダリングデータを取得する
								OutResult.RenderingData = InRenderingData;
							}
						}
					}
				}
			}
		}
	}

	return OutResult.bHit;
}
