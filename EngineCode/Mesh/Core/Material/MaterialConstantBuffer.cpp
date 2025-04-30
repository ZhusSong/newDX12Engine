// 材质常量缓冲区
#include "MaterialConstantBuffer.h"
#include "../../../Math/EngineMath.h"

FMaterialConstantBuffer::FMaterialConstantBuffer()
	:TransformInformation(EngineMath::IdentityMatrix4x4())
{
}
