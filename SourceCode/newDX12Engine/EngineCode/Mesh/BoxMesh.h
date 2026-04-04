#pragma once

#include "Core/Mesh.h"
#include "BoxMesh.CodeReflection.h"

// 盒子网格
// ボックスメッシュ
class GBoxMesh :public GMesh
{
	CODEREFLECTION()
public:
	GBoxMesh();
	virtual void Init();


	virtual void Draw(float DeltaTime);

	void CreateMesh(float InHeight, float InWidth, float InDepth);
};