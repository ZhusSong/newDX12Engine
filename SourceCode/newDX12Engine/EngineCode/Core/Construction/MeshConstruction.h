#pragma once
#include "../../Mesh/Core/MeshManager.h"

namespace MeshConstruction
{    
    // Mesh结构定义
    // Mesh構造体の定義
    template<class T, typename ...ParamTypes>
    T* CreateMeshComponent(CMeshManager* InManager, T* InMesh, ParamTypes &&...Params)
    {
        if (InManager && InMesh)
        {
            size_t HashKey = 0;
            InMesh->BuildKey(HashKey, forward<ParamTypes>(Params)...);

            std::vector<FRenderingData> RenderingDataGroup;
            if (InManager->GetRenderingPipeline()->FindMeshRenderingDataByHash(HashKey, RenderingDataGroup, (int)InMesh->GetRenderLayerType()))
            {
                InMesh->ApplyCachedMeshMetadata(HashKey);
                InManager->GetRenderingPipeline()->DuplicateMesh(InMesh, RenderingDataGroup);
            }
            else
            {
                // 提取模型资源
                 // モデルリソースを取得
                FMeshRenderingData MeshData;
                InMesh->CreateMesh(MeshData, forward<ParamTypes>(Params)...);

                // 构建mesh
                // Meshを構築
                InManager->GetRenderingPipeline()->BuildMesh(HashKey, InMesh, MeshData);
            }

            InMesh->Init();

            return InMesh;
        }

        return NULL;
    }

    template<class T, typename ...ParamTypes>
    T* CreateMeshComponent(const FCreateObjectParam& InObjectParam, 
        CMeshManager* InManager, 
        ParamTypes &&...Params)
    {
        if (InManager)
        {
            T* InMesh = CreateObject<T>(InObjectParam, new T());//NewObject

            return CreateMeshComponent<T>(InManager, InMesh, Params...);
        }

        return NULL;
    }


}



