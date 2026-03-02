#pragma once
#include "MeshConstruction.h"
#include "../../Mesh/Core/Mesh.h"
// Actor对象Mesh数据构造
// ActorオブジェクトのMeshデータ構築
namespace ActorMeshConstruction
{
	template<class T, typename ...ParamTypes>
	T* CreateMeshRenderData(CMeshManager* InManager, GMesh* InGMesh, ParamTypes &&...Params)
	{
		return MeshConstruction::CreateMeshComponent<T>(InManager, InGMesh->GetMeshComponent<T>(), Params...);
	}

	template<class T, typename ...ParamTypes>
	T* CreateMeshRenderDataByComponent(CMeshManager* InManager, T* InMeshComponent, ParamTypes &&...Params)
	{
		return MeshConstruction::CreateMeshComponent<T>(InManager, InMeshComponent, Params...);
	}
}
