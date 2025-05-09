#include "Viewport.h"
#include "../../Mesh/Core/ObjectTransform.h"
#include "../../Config/EngineRenderConfig.h"
#include "../../Math/EngineMath.h"

FViewport::FViewport()
    :ViewMatrix(EngineMath::IdentityMatrix4x4())
    , ProjectMatrix(EngineMath::IdentityMatrix4x4())
{

}

void FViewport::ViewportInit()
{
    float AspectRatio = (float)FEngineRenderConfig::GetRenderConfig()->ScrrenWidth / (float)FEngineRenderConfig::GetRenderConfig()->ScrrenHight;
    //(1,1,0) (-1,1,0) (-1,-1,0) (1,-1,0) (1,1,1) (-1,1,1) (-1,-1,1) (1,-1,1)
    // LH视角设置
    XMMATRIX Project = XMMatrixPerspectiveFovLH(
        0.25f * XM_PI, 
        AspectRatio,
        1.0f,
        1000.f
    );

    XMStoreFloat4x4(&ProjectMatrix, Project);
}
