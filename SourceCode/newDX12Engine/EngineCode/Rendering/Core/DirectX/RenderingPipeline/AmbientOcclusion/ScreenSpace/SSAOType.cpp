#include "SSAOType.h"
#include "../../../../../../Math/EngineMath.h"

FSSAOViewportTransformation::FSSAOViewportTransformation()
	:ProjectionMatrix(EngineMath::IdentityMatrix4x4())
	, InversiveProjectionMatrix(EngineMath::IdentityMatrix4x4())
	, TexProjectionMatrix(EngineMath::IdentityMatrix4x4())
{

}
