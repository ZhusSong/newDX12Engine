#pragma once
//25.6.11 李
#include "../../Mesh/Core/MeshManager.h"

// Mesh结构定义
namespace MeshConstruction
{
    template<class T, typename ...ParamTypes>
    T* CreateMeshComponent(CMeshManager* InManage, T* InMesh, ParamTypes &&...Params)
    {
        if (InManage && InMesh)
        {
            size_t HashKey = 0;
            InMesh->BuildKey(HashKey, forward<ParamTypes>(Params)...);

            std::shared_ptr<FRenderingData> RenderingData;
            if (InManage->GetRenderingPipeline().FindMeshRenderingDataByHash(HashKey, RenderingData, (int)InMesh->GetRenderLayerType()))
            {
                InManage->GetRenderingPipeline().DuplicateMesh(InMesh, RenderingData);
            }
            else
            {
                // 提取模型资源
                FMeshRenderingData MeshData;
                InMesh->CreateMesh(MeshData, forward<ParamTypes>(Params)...);

                // 构建mesh
                InManage->GetRenderingPipeline().BuildMesh(HashKey, InMesh, MeshData);
            }

            InMesh->Init();

            return InMesh;
        }

        return NULL;
    }

    template<class T, typename ...ParamTypes>
    T* CreateMeshComponent(CMeshManager* InManage, ParamTypes &&...Params)
    {
        if (InManage)
        {
            T* InMesh = CreateObject<T>(new T());//NewObject

            return CreateMeshComponent<T>(InManage, InMesh, Params...);
        }

        return NULL;
    }


}
