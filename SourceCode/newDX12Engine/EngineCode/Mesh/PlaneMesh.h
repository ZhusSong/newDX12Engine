#pragma once

#include "Core/Mesh.h"
#include "PlaneMesh.CodeReflection.h"

class GPlaneMesh :public GMesh
{
	CODEREFLECTION()
public:
	GPlaneMesh();
	virtual void Init();

	virtual void Draw(float DeltaTime);

	void CreateMesh(float InHeight, float InWidth, uint32_t InHeightSubdivide, uint32_t InWidthSubdivide);
};