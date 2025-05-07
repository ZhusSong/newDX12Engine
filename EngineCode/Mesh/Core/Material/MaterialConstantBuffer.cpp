// 材质常量缓冲区
#include "MaterialConstantBuffer.h"
#include "../../../Math/EngineMath.h"

FMaterialConstantBuffer::FMaterialConstantBuffer()
	:BaseColor(0.5f, 0.5f, 0.5f, 1.f),
	TransformInformation(EngineMath::IdentityMatrix4x4())
{
}
