#pragma once
//25.6.11 李
#include "MeshConstruction.h"
#include "../../Mesh/Core/Mesh.h"


namespace ActorMeshConstruction
{
	template<class T, typename ...ParamTypes>
	T* CreateMeshRenderData(CMeshManager* InManager, GMesh* InGMesh, ParamTypes &&...Params)
	{
		return MeshConstruction::CreateMeshComponent<T>(InManager, InGMesh->GetMeshComponent<T>(), Params...);
	}
}
