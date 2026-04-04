#pragma once

#include "Core/Mesh.h"
#include "Core/MeshType.h"
#include "PyramidMesh.CodeReflection.h"

class GPyramidMesh :public GMesh
{

	CODEREFLECTION()
public:
	GPyramidMesh();
	virtual void Init();

	virtual void Draw(float DeltaTime);

	void CreateMesh(EPyramidNumberSides InPyramidNumberSidesType, uint32_t InHeightSubdivide, uint32_t InSize = 4);
};