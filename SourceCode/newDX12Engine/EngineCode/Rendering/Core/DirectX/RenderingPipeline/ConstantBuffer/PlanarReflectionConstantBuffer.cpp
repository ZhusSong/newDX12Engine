#include "PlanarReflectionConstantBuffer.h"
#include "../../../../../Math/EngineMath.h"

FPlanarReflectionConstantBuffer::FPlanarReflectionConstantBuffer()
	: ReflectionTexViewProjection(EngineMath::IdentityMatrix4x4())
	, Settings(0.f, -1.f, 0.f, 0.f)
	, MirrorPlaneNormal(0.f, 0.f, 1.f, 0.f)
	, MirrorPlanePosition(0.f, 0.f, 0.f, 1.f)
{
}
