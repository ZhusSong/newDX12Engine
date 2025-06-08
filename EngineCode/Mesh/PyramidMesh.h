#pragma once
// 2025.6.8 李

#include "Core/Mesh.h"
#include "Core/MeshType.h"

class GPyramidMesh :public GMesh
{
	typedef GMesh Super;
public:
	virtual void Init();

	virtual void Draw(float DeltaTime);

	void CreateMesh(EPyramidNumberSides InPyramidNumberSidesType, uint32_t InHeightSubdivide, uint32_t InSize = 4);
};