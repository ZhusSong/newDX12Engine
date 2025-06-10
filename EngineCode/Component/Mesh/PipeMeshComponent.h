// 25.6.6 李
#pragma once
#include "Core/ShellMeshComponent.h"

class CPipeMeshComponent :public CShellMeshComponent
{
public:
	CPipeMeshComponent();

	void CreateMesh(FMeshRenderingData& MeshData,
		float InTopRadius,
		float InBottomRadius,
		float InHeight,
		float InThickness,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);

	void BuildKey(size_t& OutHashKey, float InTopRadius,
		float InBottomRadius,
		float InHeight,
		float InThickness,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
protected:

	// 构建半径点
	void BuildRadiusPoint(
		FMeshRenderingData& MeshData,
		float HeightInterval,
		float InTopRadius,
		float InBottomRadius,
		float BetaValue,
		float RadiusInterval,
		float InHeight,
		float InThickness,
		uint32_t InAxialSubdivision,
		uint32_t InHeightSubdivision);
};
