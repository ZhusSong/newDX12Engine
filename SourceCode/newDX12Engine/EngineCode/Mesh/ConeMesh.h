#pragma once

#include "Core/Mesh.h"
#include "ConeMesh.CodeReflection.h"

// 圆锥网格
// コーンサーフェスメッシュ
class GConeMesh :public GMesh
{
	CODEREFLECTION()
public:
	GConeMesh();
	virtual void Init();


	virtual void Draw(float DeltaTime);

	void CreateMesh(
		float InRadius,
		float InHeight,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
};