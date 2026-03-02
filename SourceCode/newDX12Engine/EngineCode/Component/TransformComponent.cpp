#include "TransformComponent.h"
#include "../Math/EngineMath.h"
// 初始化Transform组件
// Transformコンポーネントを初期化
CTransformComponent::CTransformComponent()
	:Position(0.f, 0.f, 0.f)
	, Rotation(0.f, 0.f, 0.f)
	, Scale(1.f, 1.f, 1.f)
	, ForwardVector(0.f, 0.f, 1.f)
	, RightVector(1.f, 0.f, 0.f)
	, UPVector(0.f, 1.f, 0.f)
{

}

void CTransformComponent::SetPosition(const XMFLOAT3& InNewPosition)
{
	Position = InNewPosition;
}

void CTransformComponent::SetRotation(const fvector_3d& InNewRotation)
{
	float RollRadians = XMConvertToRadians(InNewRotation.z);
	float PithRadians = XMConvertToRadians(InNewRotation.x);
	float YawRadians = XMConvertToRadians(InNewRotation.y);
	// 旋转矩阵
	// 回転行列
	XMMATRIX RotationRollPitchYawMatrix = XMMatrixRotationRollPitchYaw(
		PithRadians, YawRadians, RollRadians);

	XMVECTOR Right = XMLoadFloat3(&RightVector);
	XMVECTOR Up = XMLoadFloat3(&UPVector);
	XMVECTOR Forward = XMLoadFloat3(&ForwardVector);

	XMStoreFloat3(&RightVector, XMVector3TransformNormal(XMLoadFloat3(&RightVector), RotationRollPitchYawMatrix));
	XMStoreFloat3(&UPVector, XMVector3TransformNormal(XMLoadFloat3(&UPVector), RotationRollPitchYawMatrix));
	XMStoreFloat3(&ForwardVector, XMVector3TransformNormal(XMLoadFloat3(&ForwardVector), RotationRollPitchYawMatrix));
}

void CTransformComponent::SetRotation(const frotator& InNewRotation)
{
	float RollRadians = XMConvertToRadians(InNewRotation.roll);
	float PithRadians = XMConvertToRadians(InNewRotation.pitch);
	float YawRadians = XMConvertToRadians(InNewRotation.yaw);

	// 旋转矩阵
	// 回転行列
	XMMATRIX RotationRollPitchYawMatrix = XMMatrixRotationRollPitchYaw(
		PithRadians, YawRadians, RollRadians);

	// 还原为最原始的
	// 元の状態に戻す
	XMVECTOR Right = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	XMVECTOR Up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR Forward = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	XMStoreFloat3(&RightVector, XMVector3TransformNormal(Right, RotationRollPitchYawMatrix));
	XMStoreFloat3(&UPVector, XMVector3TransformNormal(Up, RotationRollPitchYawMatrix));
	XMStoreFloat3(&ForwardVector, XMVector3TransformNormal(Forward, RotationRollPitchYawMatrix));
}


void CTransformComponent::SetRotationQuat(const fquat& InNewQuatRotation)
{
	fmatrix_3x3 MatrixRotation;
	math_utils::object_to_inertia(InNewQuatRotation, MatrixRotation);

	fvector_3d Right = fvector_3d(1.f, 0.f, 0.f);
	fvector_3d Up = fvector_3d(0.f, 1.f, 0.f);
	fvector_3d Forward = fvector_3d(0.f, 0.f, 1.f);

	Right = math_utils::mul(Right, MatrixRotation);
	Up = math_utils::mul(Up, MatrixRotation);
	Forward = math_utils::mul(Forward, MatrixRotation);

	Right.normalize();
	Up.normalize();
	Forward.normalize();

	RightVector = EngineMath::ToFloat3(Right);
	UPVector = EngineMath::ToFloat3(Up);
	ForwardVector = EngineMath::ToFloat3(Forward);
}
void CTransformComponent::SetScale(const fvector_3d& InNewScale)
{
	Scale.x = InNewScale.x;
	Scale.y = InNewScale.y;
	Scale.z = InNewScale.z;
}
void CTransformComponent::SetForwardVector(const XMFLOAT3& InForwardVector)
{
	ForwardVector = InForwardVector;
}

void CTransformComponent::SetRightVector(const XMFLOAT3& InRightVector)
{
	RightVector = InRightVector;
}

void CTransformComponent::SetUPVector(const XMFLOAT3& InUPVector)
{
	UPVector = InUPVector;
}

frotator CTransformComponent::GetRotation() const
{
	frotator Rotator;
	fmatrix_3x3 RotatorMatrix;
	EngineMath::BuildRotatorMatrix(
		RotatorMatrix,
		RightVector,
		UPVector,
		ForwardVector);

	Rotator.inertia_to_object(RotatorMatrix);
	// 转为DirectX的旋转矩阵
	// DirectXの回転行列に変換
	return EngineMath::ToDXRotator(Rotator);
}

fquat CTransformComponent::GetRotationQuat() const
{
	fquat Quat;

	fmatrix_3x3 RotatorMatrix;
	EngineMath::BuildRotatorMatrix(
		RotatorMatrix,
		RightVector,
		UPVector,
		ForwardVector);

	math_utils::matrix_to_quat(RotatorMatrix, Quat);

	return Quat;
}
// 矫正位置矩阵
// 位置補正行列
void CTransformComponent::CorrectionVector()
{
	XMVECTOR Right = XMLoadFloat3(&RightVector);
	XMVECTOR Up = XMLoadFloat3(&UPVector);
	XMVECTOR Forward = XMLoadFloat3(&ForwardVector);

	Forward = XMVector3Normalize(Forward);
	Up = XMVector3Normalize(XMVector3Cross(Forward, Right));

	Right = XMVector3Cross(Up, Forward);

	XMStoreFloat3(&RightVector, Right);
	XMStoreFloat3(&UPVector, Up);
	XMStoreFloat3(&ForwardVector, Forward);
}

fvector_3d CTransformComponent::GetCorrectionPosition()
{
	fvector_3d F3D;
	GetCorrectionPosition(F3D);

	return F3D;
}

void CTransformComponent::GetCorrectionPosition(fvector_3d& InPos3)
{
	XMVECTOR Right = XMLoadFloat3(&RightVector);
	XMVECTOR Up = XMLoadFloat3(&UPVector);
	XMVECTOR Forward = XMLoadFloat3(&ForwardVector);

	XMVECTOR NewPosition = XMLoadFloat3(&Position);

	InPos3.x = -XMVectorGetX(XMVector3Dot(NewPosition, Right));
	InPos3.y = -XMVectorGetX(XMVector3Dot(NewPosition, Up));
	InPos3.z = -XMVectorGetX(XMVector3Dot(NewPosition, Forward));
}
