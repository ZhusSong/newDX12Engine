#include "EngineMath.h"

namespace EngineMath
{
    XMFLOAT4X4 IdentityMatrix4x4()
    {
        return XMFLOAT4X4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    XMFLOAT4 ToFloat4(const fvector_4d& InV4d)
    {
        return XMFLOAT4(InV4d.x, InV4d.y, InV4d.z, InV4d.w);
    }

    XMFLOAT3 ToFloat3(const fvector_3d& InV3d)
    {
        return XMFLOAT3(InV3d.x, InV3d.y, InV3d.z);
    }

    fvector_3d ToVector3d(const XMFLOAT3& InV3d)
    {
        return fvector_3d(InV3d.x, InV3d.y, InV3d.z);
    }


    bool IsAngleRange(float InAngle, float X, float Y)
    {
        if (InAngle >= min(X, Y))
        {
            if (InAngle <= max(X, Y))
            {
                return true;
            }
        }

        return false;
    }

    fvector_3d GetPointSphericalCoordinates(const fvector_3d& InPoint)
    {
        float AnglePre = (180.f / 3.1415926f);

        float R = sqrt(InPoint.x * InPoint.x + InPoint.y * InPoint.y + InPoint.z * InPoint.z);
        float Thata = acos(InPoint.y / R) * AnglePre;
        float Fai = atan2f(InPoint.z, InPoint.x) * AnglePre;

        return fvector_3d(R, Thata, Fai);
    }

    struct FCubeMapAxialRangeR
    {
        // 定义一个cubemap贴图各个面在球面上的范围
        FCubeMapAxialRangeR()
            :PositiveX(45.f, 135.f, 45.f, -45.f)
            , NegativeX(45.f, 135.f, 135.f, -135.f)
            , PositiveY(0.f, 45.f, 360.f, -360.f)
            , NegativeY(135.f, 180.f, 360.f, -360.f)
            , PositiveZ(45.f, 135.f, 45.f, 135.f)
            , NegativeZ(45.f, 135.f, -45.f, -135.f)
        {}

        const fvector_4d PositiveX;
        const fvector_4d NegativeX;

        const fvector_4d PositiveY;
        const fvector_4d NegativeY;

        const fvector_4d PositiveZ;
        const fvector_4d NegativeZ;
    };

    bool IsAngleAxisRange(
        float InAngle,
        float X,
        float Y,
        float InCriticalValue,
        bool bComMin)
    {
        if (bComMin) //比小
        {
            if (InAngle > InCriticalValue)
            {
                if (InAngle <= max(X, Y))
                {
                    return true;
                }
            }
            else if (InAngle < InCriticalValue)
            {
                if (InAngle >= min(X, Y))
                {
                    return true;
                }
            }
            else if (InAngle == 0.f)
            {
                return true;
            }
        }
        else
        {
            if (InAngle > 0.f) //+
            {
                if (InAngle >= max(X, Y))
                {
                    if (InAngle <= InCriticalValue)
                    {
                        return true;
                    }
                }
            }
            else
            {
                if (InAngle <= min(X, Y))
                {
                    if (InAngle >= -InCriticalValue)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    // 判断一个点是否在视口中
    bool IsPointInCubeMapVieport(
        float InPointTheta,
        float InPointFai,
        fvector_4d InCompareValue,
        int bAxisRange = 0,
        float InCriticalValue = 0.f,
        bool bComMin = false)
    {
        if (IsAngleRange(InPointTheta, InCompareValue.x, InCompareValue.y))
        {
            if (bAxisRange == 0)
            {
                if (IsAngleAxisRange(InPointFai, InCompareValue.z, InCompareValue.w, InCriticalValue, bComMin))
                {
                    return true;
                }
            }
            else if (bAxisRange == 1)
            {
                if (InPointFai >= InCompareValue.z)
                {
                    if (InPointFai <= InCompareValue.w)
                    {
                        return true;
                    }
                }
            }
            else if (bAxisRange == 2)
            {
                if (InPointFai <= InCompareValue.z)
                {
                    if (InPointFai >= InCompareValue.w)
                    {
                        return true;
                    }
                }
            }
            else
            {
                if (IsAngleRange(InPointFai, InCompareValue.z, InCompareValue.w))
                {
                    return true;
                }
            }
        }

        return false;
    }

    ECubeMapFaceType GetSampleCubeMapIndexR(const fvector_3d& InPointPosition)
    {
        static FCubeMapAxialRangeR CubeMapAxialRangeRight;

        // 确保它已经转为CubeMapViewport下的坐标
        // 转为球面坐标
        fvector_3d Point = GetPointSphericalCoordinates(InPointPosition);

        // 球面坐标值
        float PointTheta = Point.y;
        float PointFai = Point.z;

        if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.PositiveX, 3, 0.f, true))
        {
            return ECubeMapFaceType::POSITIVE_X;
        }
        else if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.NegativeX, 0, 180.f, false))
        {
            return ECubeMapFaceType::NEGATIVE_X;
        }
        else if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.PositiveY, 3))
        {
            return ECubeMapFaceType::POSITIVE_Y;
        }
        else if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.NegativeY, 3))
        {
            return ECubeMapFaceType::NEGATIVE_Y;
        }
        else if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.PositiveZ, 1))
        {
            return ECubeMapFaceType::POSITIVE_Z;
        }
        else if (IsPointInCubeMapVieport(PointTheta, PointFai, CubeMapAxialRangeRight.NegativeZ, 2))
        {
            return ECubeMapFaceType::NEGATIVE_Z;
        }

        return ECubeMapFaceType::NEGATIVE_INVALID;
    }
}

