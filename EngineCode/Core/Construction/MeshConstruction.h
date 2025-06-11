#pragma once
//25.6.11 李
#include "../../Mesh/Core/MeshManager.h"

// Mesh结构定义
namespace MeshConstruction
{
    template<class T, typename ...ParamTypes>
    T* CreateMeshComponent(CMeshManager* InManager, T* InMesh, ParamTypes &&...Params)
    {
        if (InManager && InMesh)
        {
            size_t HashKey = 0;
            InMesh->BuildKey(HashKey, forward<ParamTypes>(Params)...);

            FRenderingData RenderingData;
            if (InManager->GetRenderingPipeline().FindMeshRenderingDataByHash(HashKey, RenderingData, (int)InMesh->GetRenderLayerType()))
            {
                InManager->GetRenderingPipeline().DuplicateMesh(InMesh, RenderingData);
            }
            else
            {
                //提取模型资源
                FMeshRenderingData MeshData;
                InMesh->CreateMesh(MeshData, forward<ParamTypes>(Params)...);

                //构建mesh
                InManager->GetRenderingPipeline().BuildMesh(HashKey, InMesh, MeshData);
            }

            InMesh->Init();

            return InMesh;
        }

        return NULL;
    }

    template<class T, typename ...ParamTypes>
    T* CreateMeshComponent(CMeshManager* InManager, ParamTypes &&...Params)
    {
        if (InManager)
        {
            T* InMesh = CreateObject<T>(new T());

            return CreateMeshComponent<T>(InManager, InMesh, Params...);
        }

        return NULL;
    }


}
