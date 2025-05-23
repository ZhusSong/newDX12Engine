﻿// 25.5.16 李
#pragma once
#include "ShellMeshComponent.h"

class CConeMeshComponent :public CShellMeshComponent
{
public:
	CConeMeshComponent();

	void CreateMesh(
		FMeshRenderingData& MeshData,
		float InRadius,
		float InHeight,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
};
