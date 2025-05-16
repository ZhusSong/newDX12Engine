// 2025.4.李
#pragma once

#include "Core/Mesh.h"

class GPlaneMesh :public GMesh
{
	typedef GMesh Super;
public:
	virtual void Init();


	virtual void Draw(float DeltaTime);


	void CreateMesh(float InHeight, float InWidth, uint32_t InHeightSubdivide, uint32_t InWidthSubdivide);
};