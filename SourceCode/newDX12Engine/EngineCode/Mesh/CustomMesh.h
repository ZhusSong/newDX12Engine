#pragma once

#include "Core/Mesh.h"
#include "CustomMesh.CodeReflection.h"

// 读取obj模型mesh
// objモデルのメッシュを読み込む
class GCustomMesh :public GMesh
{
	CODEREFLECTION()
public:
	GCustomMesh();
	virtual void Init();


	virtual void Draw(float DeltaTime);


	void CreateMesh(string& InPath);
};
