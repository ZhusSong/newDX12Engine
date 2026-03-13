#pragma once
#include "../EngineMinimal.h"
#include <DirectXMath.h>

// 数学相关
// 数学関連
namespace EngineMath
{
	enum ECubeMapFaceType
	{
		POSITIVE_X,//X
		NEGATIVE_X,//-X
		POSITIVE_Y,//Y
		NEGATIVE_Y,//-Y
		POSITIVE_Z,//Z
		NEGATIVE_Z,//-Z
		NEGATIVE_INVALID,
	};

	bool IsRange(float InMax, int InMin, int InValue);

	XMFLOAT4X4 IdentityMatrix4x4();

	XMFLOAT4 ToFloat4(const fvector_4d& InV4d);
	XMFLOAT3 ToFloat3(const fvector_3d& InV3d);
	fvector_3d ToVector3d(const XMFLOAT3& InV3d);

	fmatrix_4x4 ToMatrix4x4(const XMFLOAT4X4& InMatrix4x4);
	XMFLOAT4X4 ToFloat4x4(const fmatrix_4x4& InMatrix4x4);

	fmatrix_3x3 ToMatrix3x3(const XMFLOAT3X3& InMatrix3x3);
	XMFLOAT3X3 ToFloat3x3(const fmatrix_3x3& InMatrix3x3);

	bool IsAngleRange(float InAngle, float X, float Y);
	fvector_3d GetPointSphericalCoordinates(const fvector_3d& InPoint);
	ECubeMapFaceType GetSampleCubeMapIndexR(const fvector_3d& InPointPosition);


	fquat BuildQuat(const fvector_3d& InForwardVector, const fvector_3d& InUPVector = fvector_3d(0.f, 1.f, 0.f));

	frotator BuildRotatorMatrix(const fvector_3d& InForwardVector, const fvector_3d& InUPVector = fvector_3d(0.f, 1.f, 0.f));

	// 与DirectX的旋转矩阵做转换
	// DirectXの回転行列との変換を行う
	frotator ToDXRotator(const frotator& InRotator);

	void BuildRotatorMatrix(
		fmatrix_3x3& OutMatrix,
		const XMFLOAT3& InRightVector,
		const XMFLOAT3& InUPVector,
		const XMFLOAT3& InForwardVector);

	void BuildRotatorMatrix(
		fmatrix_3x3& OutMatrix,
		const fvector_3d& InRightVector,
		const fvector_3d& InUPVector,
		const fvector_3d& InForwardVector);

	void BuildMatrix(
		XMFLOAT4X4& OutMatrix,
		const XMFLOAT3& InPosition,
		const fvector_3d& InScale,
		const XMFLOAT3& InRightVector, const XMFLOAT3& InUPVector, const XMFLOAT3& InForwardVector);
	
	void BuildInverseMatrix(
		XMMATRIX& OutMatrix,
		const XMFLOAT3& InPosition,
		const fvector_3d& InScale,
		const XMFLOAT3& InRightVector, const XMFLOAT3& InUPVector, const XMFLOAT3& InForwardVector);

	// 球面坐标采样
	// 球面座標のサンプリング
	int GetSample8CubeIndex(const fvector_3d& InRelativePointPosition);

	// 插值
	// 補間
	template<class T>
	static T Lerp(const T& InA, const T& InB, float InTime)
	{
		return InA + (InB - InA) * InTime;
	}
}
