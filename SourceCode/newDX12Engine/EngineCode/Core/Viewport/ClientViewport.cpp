#include "ClientViewport.h"
#include "../../Component/TransformComponent.h"
#include "../../Math/EngineMath.h"

GClientViewport::GClientViewport()
    :SuperV()
    , SuperA()
    , bDirty(true)
{
}

void GClientViewport::SetFrustum(float InYFOV, float InZNear, float InZFar)
{
    SetFrustum(InYFOV, ViewportData.Aspect, InZNear, InZFar);
}

void GClientViewport::SetFrustum(float InYFOV, float InAspect, float InZNear, float InZFar)
{
    ViewportData.YFOV = InYFOV;
    ViewportData.Aspect = InAspect;
    ViewportData.ZNear = InZNear;
    ViewportData.ZFar = InZFar;

#if USE_SIMPLE_LIB_MATH
    ProjectMatrix = EngineMath::ToFloat4x4(math_utils::matrix_perspective(
        InYFOV, //以弧度为单位的自上而下的视场角。   // 上から下への視野角（ラジアン単位）
        InAspect,//视图空间 X:Y 的纵横比。           // ビュースペースのX:Yアスペクト比
        InZNear,//到近剪裁平面的距离。必须大于零。   // 近クリップ平面までの距離（0より大きい必要あり）
        InZFar//到远剪裁平面的距离。必须大于零。     // 遠クリップ平面までの距離（0より大きい必要あり）
    ));
#else    
    // 基于视野构建左手透视投影矩阵
    // 視野に基づいて左手系透視投影行列を構築
    XMMATRIX Project = XMMatrixPerspectiveFovLH(
        InYFOV, //以弧度为单位的自上而下的视场角。   // 上から下への視野角（ラジアン単位）
        InAspect,//视图空间 X:Y 的纵横比。           // ビュースペースのX:Yアスペクト比
        InZNear,//到近剪裁平面的距离。必须大于零。   // 近クリップ平面までの距離（0より大きい必要あり）
        InZFar//到远剪裁平面的距离。必须大于零。     // 遠クリップ平面までの距離（0より大きい必要あり）
    );

    XMStoreFloat4x4(&ProjectMatrix, Project);
#endif // USE_SIMPLE_LIB_MATH
    SetDirty(true);
}


void GClientViewport::FaceTarget(
    const fvector_3d& InPosition,
    const fvector_3d& InTargetPosition,
    const fvector_3d& InUP)
{
    // 拿到三个方向
    // 3つの方向ベクトルを取得
    fvector_3d TmpFaceVector = InTargetPosition - InPosition;
    TmpFaceVector.normalize();

    fvector_3d TmpRightVector = fvector_3d::cross_product(InUP, TmpFaceVector);
    TmpRightVector.normalize();

    fvector_3d TmpUPVector = fvector_3d::cross_product(TmpFaceVector, TmpRightVector);
    TmpUPVector.normalize();

    // 转换
    // 変換
    XMFLOAT3 XMFaceVector = EngineMath::ToFloat3(TmpFaceVector);
    XMFLOAT3 XMRightVector = EngineMath::ToFloat3(TmpRightVector);
    XMFLOAT3 XMUPVector = EngineMath::ToFloat3(TmpUPVector);

    // 赋值操作
    // 値を代入
    GetForwardVector() = XMFaceVector;
    GetRightVector() = XMRightVector;
    GetUPVector() = XMUPVector;

    SetDirty(true);
}

void GClientViewport::Tick(float DeltaTime)
{
    BuildViewMatrix(DeltaTime);
}

void GClientViewport::OnResetSize(int InWidth, int InHeight)
{
    SuperV::OnResetSize(InWidth, InHeight);

    ViewportData.ResetSize(InWidth, InHeight);
}

void GClientViewport::BuildViewMatrix(float DeltaTime)
{
    // 计算和矫正轴
    // 軸を計算して補正
    GetRootComponent()->CorrectionVector();

    // 算出按自身方向移动意图
    // 自身方向に沿った移動意図を計算
    fvector_3d V3;
    GetRootComponent()->GetCorrectionPosition(V3);

    // 构建Viewmatrix
    // View行列を構築
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
     // ライトビュー空間に変換
    XMFLOAT3 ViewCenter;
    XMStoreFloat3(&ViewCenter, XMVector3TransformCoord(TargetPositionTOR, ShadowViewMatrixRTX));

    // 正交关系
    // 正交関係
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
    // 保存
    XMStoreFloat4x4(&ProjectMatrix, ProjectMatrixRIX);
}