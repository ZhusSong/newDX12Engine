#pragma once
//25.6.11 李
#include "MeshConstruction.h"
#include "../../Mesh/Core/Mesh.h"

// Actor对象Mesh数据构造
namespace ActorMeshConstruction
{
	template<class T, typename ...ParamTypes>
	T* CreateMeshRenderData(CMeshManager* InManager, GMesh* InGMesh, ParamTypes &&...Params)
	{
		return MeshConstruction::CreateMeshComponent<T>(InManager, InGMesh->GetMeshComponent<T>(), Params...);
	}

	template<class T, typename ...ParamTypes>
	T* CreateMeshRenderDataByComponent(CMeshManager* InManage, T* InMeshComponent, ParamTypes &&...Params)
	{
		return MeshConstruction::CreateMeshComponent<T>(InManage, InMeshComponent, Params...);
	}
}
