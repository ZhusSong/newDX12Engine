#pragma once

#include "Core/Mesh.h"

// 读取obj模型mesh
class GCustomMesh :public GMesh
{
	typedef GMesh Super;
public:
	virtual void Init();


	virtual void Draw(float DeltaTime);


	void CreateMesh(string& InPath);
};
