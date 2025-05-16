#pragma once

#include "Core/Mesh.h"

// 圆锥网格
class GConeMesh :public GMesh
{
	typedef GMesh Super;
public:
	virtual void Init();


	virtual void Draw(float DeltaTime);

	void CreateMesh(
		float InRadius,
		float InHeight,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
};