#pragma once
#include "../EngineMinimal.h"
#include <DirectXMath.h>

// 数学相关
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
		NEGATIVE_INVALID,//无效
	};

	XMFLOAT4X4 IdentityMatrix4x4();

	XMFLOAT4 ToFloat4(const fvector_4d& InV4d);
	XMFLOAT3 ToFloat3(const fvector_3d& InV3d);
	fvector_3d ToVector3d(const XMFLOAT3& InV3d);

	// 检查角度InAngle是否在范围内
	bool IsAngleRange(float InAngle, float X, float Y);
	
	// 将3D空间向量转为球面坐标（极坐标）
	fvector_3d GetPointSphericalCoordinates(const fvector_3d& InPoint);
	
	// 将球面上某点转为球面坐标并判断应将其映射到cubemap上的哪个面(右手系)
	ECubeMapFaceType GetSampleCubeMapIndexR(const fvector_3d& InPointPosition);
}