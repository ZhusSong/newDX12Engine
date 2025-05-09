// 2025.4.李
#include "MeshType.h"
#include "MeshManager.h"

FVertex::FVertex(const XMFLOAT3& InPos, const XMFLOAT4& InColor)
	:Position(InPos)
	, Color(InColor)
{

}

FVertex::FVertex(const XMFLOAT3& InPos, const XMFLOAT4& InColor, const XMFLOAT3& InNormal)
:Position(InPos)
, Color(InColor)
, Normal(InNormal)
{
}
