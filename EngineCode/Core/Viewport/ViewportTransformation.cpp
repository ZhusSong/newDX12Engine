﻿
// 2025.4.李
#include "ViewportTransformation.h"

FViewportTransformation::FViewportTransformation()
    :ViewportPosition(0.f, 0.f, 0.f, 1.f),
    ViewProjectionMatrix(EngineMath::IdentityMatrix4x4())
{

}