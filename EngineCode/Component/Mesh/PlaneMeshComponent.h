﻿// 25.5.16 李
#pragma once
#include "ShellMeshComponent.h"

class CPlaneMeshComponent :public CShellMeshComponent
{
public:
	CPlaneMeshComponent();
	
	void CreateMesh(FMeshRenderingData& MeshData, float InHeight, float InWidth, uint32_t InHeightSubdivide, uint32_t InWidthSubdivide);
};
