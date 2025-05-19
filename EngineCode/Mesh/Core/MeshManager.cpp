#include "MeshManager.h"
#include "../../Config/EngineRenderConfig.h"
#include "ObjectTransform.h"
#include "../../Rendering/Core/RenderingResourcesUpdate.h"
#include "../../Rendering/Engine/DirectX/Core/DirectXRenderingEngine.h"
#include "../../Rendering/Core/Buffer/ConstructBuffer.h"
#include "../../Math/EngineMath.h"
#include "../../Core/Viewport/ViewportTransformation.h"
#include "../../Component/Mesh/BoxMeshComponent.h"
#include "../../Component/Mesh/ConeMeshComponent.h"
#include "../../Component/Mesh/CustomMeshComponent.h"
#include "../../Component/Mesh/CylinderMeshComponent.h"
#include "../../Component/Mesh/PlaneMeshComponent.h"
#include "../../Component/Mesh/SphereMeshComponent.h"

CMeshManager::CMeshManager()
{

}

void CMeshManager::Init()
{
   
}

void CMeshManager::BuildMesh()
{
    // 创建渲染流水线
    RenderingPipeline.BuildPipeline();
}


void CMeshManager::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{

    RenderingPipeline.UpdateCalculations(DeltaTime, ViewportInfo);
}

void CMeshManager::PostDraw(float DeltaTime)
{
    RenderingPipeline.PostDraw(DeltaTime);

}

void CMeshManager::Draw(float DeltaTime)
{
    RenderingPipeline.Draw(DeltaTime);
   
}

void CMeshManager::PreDraw(float DeltaTime)
{
    RenderingPipeline.PreDraw(DeltaTime);
}

CMeshComponent* CMeshManager::CreateSphereMeshComponent(float InRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
    return CreateMeshComponent<CSphereMeshComponent>(InRadius, InAxialSubdivision, InHeightSubdivision);
}

CMeshComponent* CMeshManager::CreateMeshComponent(string& InPath)
{
    return CreateMeshComponent<CCustomMeshComponent>(InPath);
}

CMeshComponent* CMeshManager::CreateBoxMeshComponent(float InHeight, float InWidth, float InDepth)
{
    return CreateMeshComponent<CBoxMeshComponent>(InHeight, InWidth, InDepth);
}

CMeshComponent* CMeshManager::CreateConeMeshComponent(float InRadius, float InHeight, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
    return CreateMeshComponent<CConeMeshComponent>(InRadius, InHeight, InAxialSubdivision, InHeightSubdivision);
}

CMeshComponent* CMeshManager::CreateCylinderMeshComponent(float InTopRadius, float InBottomRadius, float InHeight, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
    return CreateMeshComponent<CCylinderMeshComponent>(InTopRadius, InBottomRadius, InHeight, InAxialSubdivision, InHeightSubdivision);
}

CMeshComponent* CMeshManager::CreatePlaneMeshComponent(float InHeight, float InWidth, uint32_t InHeightSubdivide, uint32_t InWidthSubdivide)
{
    return CreateMeshComponent<CPlaneMeshComponent>(InHeight, InWidth, InHeightSubdivide, InWidthSubdivide);
}


template<class T, typename ...ParamTypes>
T* CMeshManager::CreateMeshComponent(ParamTypes && ...Params)
{
    T* MyMesh = CreateObject<T>(new T());

    //提取模型资源
    FMeshRenderingData MeshData;
    MyMesh->CreateMesh(MeshData, forward<ParamTypes>(Params)...);


    //构建mesh
    RenderingPipeline.BuildMesh(MyMesh, MeshData);

    MyMesh->Init();

    return MyMesh;
}