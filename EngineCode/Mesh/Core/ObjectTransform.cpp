#include "ObjectTransform.h"

FObjectTransform::FObjectTransform()
    :World(FObjectTransform::IdentityMatrix4x4())
{
}

XMFLOAT4X4 FObjectTransform::IdentityMatrix4x4()
{
    return XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}