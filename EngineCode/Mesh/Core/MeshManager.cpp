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
#include "../../Component/Mesh/PyramidMeshComponent.h"
#include "../../Component/Mesh/PipeMeshComponent.h"
#include "../../Component/Mesh/TorusMeshComponent.h"


#include "../../Core/Construction/MeshConstruction.h"


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

CMeshComponent* CMeshManager::CreateSphereMeshComponent(float InRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision, bool bReverse)
{
    return MeshConstruction::CreateMeshComponent<CSphereMeshComponent>(this, InRadius, InAxialSubdivision, InHeightSubdivision, bReverse);
}


CMeshComponent* CMeshManager::CreatePyramidMeshComponent(EPyramidNumberSides InPyramidNumberSidesType, uint32_t InHeightSubdivide, uint32_t InSize)
{
    return MeshConstruction::CreateMeshComponent<CPyramidMeshComponent>(this, InPyramidNumberSidesType, InHeightSubdivide, InSize);
}

CMeshComponent* CMeshManager::CreatePipeMeshComponent(float InTopRadius, float InBottomRadius, float InHeight, float InThickness, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
    return MeshConstruction::CreateMeshComponent<CPipeMeshComponent>(this, InTopRadius, InBottomRadius, InHeight, InThickness, InAxialSubdivision, InHeightSubdivision);
}

CMeshComponent* CMeshManager::CreateTorusMeshComponent(float InRadius, float InSectionRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
    return MeshConstruction::CreateMeshComponent<CTorusMeshComponent>(this, InRadius, InSectionRadius, InAxialSubdivision, InHeightSubdivision);
}

CMeshComponent* CMeshManager::CreateMeshComponent(string& InPath)
{
    return MeshConstruction::CreateMeshComponent<CCustomMeshComponent>(this, InPath);
}

CMeshComponent* CMeshManager::CreateBoxMeshComponent(float InHeight, float InWidth, float InDepth)
{
    return MeshConstruction::CreateMeshComponent<CBoxMeshComponent>(this, InHeight, InWidth, InDepth);
}

CMeshComponent* CMeshManager::CreateConeMeshComponent(float InRadius, float InHeight, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
    return MeshConstruction::CreateMeshComponent<CConeMeshComponent>(this, InRadius, InHeight, InAxialSubdivision, InHeightSubdivision);
}

CMeshComponent* CMeshManager::CreateCylinderMeshComponent(float InTopRadius, float InBottomRadius, float InHeight, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
    return MeshConstruction::CreateMeshComponent<CCylinderMeshComponent>(this, InTopRadius, InBottomRadius, InHeight, InAxialSubdivision, InHeightSubdivision);
}

CMeshComponent* CMeshManager::CreatePlaneMeshComponent(float InHeight, float InWidth, uint32_t InHeightSubdivide, uint32_t InWidthSubdivide)
{
    return MeshConstruction::CreateMeshComponent<CPlaneMeshComponent>(this, InHeight, InWidth, InHeightSubdivide, InWidthSubdivide);
}
