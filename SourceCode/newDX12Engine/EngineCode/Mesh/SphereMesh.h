
#pragma once

#include "Core/Mesh.h"
#include "SphereMesh.CodeReflection.h"

class GSphereMesh :public GMesh
{
	CODEREFLECTION()
public:
	GSphereMesh();
	virtual void Init();


	virtual void Draw(float DeltaTime);


	void CreateMesh(float InRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision, bool bReverse = false);
};