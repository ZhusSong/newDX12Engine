﻿// 材质常量缓冲区
#include "MaterialConstantBuffer.h"
#include "../../../Math/EngineMath.h"

// 此处进行初始化,需与shader中定义一致
FMaterialConstantBuffer::FMaterialConstantBuffer()
	:MaterialType(0)
	, BaseColor(1.f, 1.f, 1.f, 1.f)
	, Roughness(0.2f)
	, TransformInformation(EngineMath::IdentityMatrix4x4())
{
}
