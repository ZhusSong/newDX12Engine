// 25.5.16 李
#pragma once
#include "ShellMeshComponent.h"

class CBoxMeshComponent :public CShellMeshComponent
{
public:
	CBoxMeshComponent();

	void CreateMesh(FMeshRenderingData& MeshData, float InHeight, float InWidth, float InDepth);
};
