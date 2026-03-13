// 材质常量缓冲区
// マテリアル定数バッファ
#include "MaterialConstantBuffer.h"
#include "../../../Math/EngineMath.h"

// 此处进行初始化,需与shader中定义一致
// ここで初期化を行う。Shader内の定義と一致させる必要がある
FMaterialConstantBuffer::FMaterialConstantBuffer()
	:MaterialType(0)
	, BaseColor(1.f, 1.f, 1.f, 1.f)
	, Roughness(0.2f)
	, BaseColorIndex(-1)
	, NormalIndex(-1)
	, SpecularIndex(-1)
	, FresnelF0(0.04f, 0.04f, 0.04f)
	, Transparency(1.f)
	, TransformInformation(EngineMath::IdentityMatrix4x4())
	, Refraction(1.1f)
	, Metallicity(0.2f, 0.2f, 0.2f)
{
}
