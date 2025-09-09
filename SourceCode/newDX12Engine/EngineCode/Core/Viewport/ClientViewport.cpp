#include "ClientViewport.h"
#include "../../Component/TransformComponent.h"
#include "../../Math/EngineMath.h"

GClientViewport::GClientViewport()
    :SuperV()
    , SuperA()
    , YFOV(0.f)
    , Aspect(0.f)
    , ZNear(0.f)
    , ZFar(0.f)
    , bDirty(true)
{
}

void GClientViewport::SetFrustum(float InYFOV, float InAspect, float InZNear, float InZFar)
{
    YFOV = InYFOV;
    Aspect = InAspect;
    ZNear = InZNear;
    ZFar = InZFar;

    // 基于视野构建左手透视投影矩阵
    XMMATRIX Project = XMMatrixPerspectiveFovLH(
        InYFOV, //以弧度为单位的自上而下的视场角。
        InAspect,//视图空间 X:Y 的纵横比。
        InZNear,//到近剪裁平面的距离。必须大于零。
        InZFar//到远剪裁平面的距离。必须大于零。
    );

    XMStoreFloat4x4(&ProjectMatrix, Project);

    SetDirty(true);
}

void GClientViewport::FaceTarget(
    const fvector_3d& InPosition,
    const fvector_3d& InTargetPosition,
    const fvector_3d& InUP)
{
    // 计算前向、右向、上向向量
    fvector_3d TmpFaceVector = InTargetPosition - InPosition;
    TmpFaceVector.normalize();

    fvector_3d TmpRightVector = fvector_3d::cross_product(InUP, TmpFaceVector);
    TmpRightVector.normalize();

    fvector_3d TmpUPVector = fvector_3d::cross_product(TmpFaceVector, TmpRightVector);
    TmpUPVector.normalize();

    // 转换为 DirectX 格式并存储
    XMFLOAT3 XMFaceVector = EngineMath::ToFloat3(TmpFaceVector);
    XMFLOAT3 XMRightVector = EngineMath::ToFloat3(TmpRightVector);
    XMFLOAT3 XMUPVector = EngineMath::ToFloat3(TmpUPVector);

    // 赋值操作
    GetForwardVector() = XMFaceVector;
    GetRightVector() = XMRightVector;
    GetUPVector() = XMUPVector;

    SetDirty(true);
}

void GClientViewport::Tick(float DeltaTime)
{
    BuildViewMatrix(DeltaTime);
}

void GClientViewport::BuildViewMatrix(float DeltaTime)
{
    // 计算和矫正轴
    GetRootComponent()->CorrectionVector();

    // 算出按自身方向移动意图
    fvector_3d V3;
    GetRootComponent()->GetCorrectionPosition(V3);

    // 构建Viewmatrix
    XMFLOAT3 RightVector = GetRootComponent()->GetRightVector();
    XMFLOAT3 UPVector = GetRootComponent()->GetUPVector();
    XMFLOAT3 ForwardVector = GetRootComponent()->GetForwardVector();

    ViewMatrix = {
        RightVector.x,	UPVector.x,	ForwardVector.x,	0.f,
        RightVector.y,	UPVector.y,	ForwardVector.y,	0.f,
        RightVector.z,	UPVector.z,	ForwardVector.z,	0.f,
        V3.x,			V3.y,		V3.z,				1.f };
}

void GClientViewport::BuildOrthographicOffCenterLHMatrix(float InRadius, const fvector_3d& InTargetPosition)
{
    XMFLOAT3 TargetPosition = EngineMath::ToFloat3(InTargetPosition);
    XMVECTOR TargetPositionTOR = XMLoadFloat3(&TargetPosition);

    XMMATRIX ShadowViewMatrixRTX = XMLoadFloat4x4(&ViewMatrix);

    // 变换到灯光视口空间
    XMFLOAT3 ViewCenter;
    XMStoreFloat3(&ViewCenter, XMVector3TransformCoord(TargetPositionTOR, ShadowViewMatrixRTX));

    // 正交关系
    float ViewLeft = ViewCenter.x - InRadius;
    float ViewRight = ViewCenter.x + InRadius;
    float ViewBottom = ViewCenter.y - InRadius;
    float ViewTop = ViewCenter.y + InRadius;
    float NearZ = ViewCenter.z - InRadius;
    float FarZ = ViewCenter.z + InRadius;

    XMMATRIX ProjectMatrixRIX = XMMatrixOrthographicOffCenterLH(
        ViewLeft,
        ViewRight,
        ViewBottom,
        ViewTop,
        NearZ,
        FarZ);

    // 存储
    XMStoreFloat4x4(&ProjectMatrix, ProjectMatrixRIX);
}