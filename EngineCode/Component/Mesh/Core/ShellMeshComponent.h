// 25.6.6 李
#pragma once
#include "MeshComponent.h"

// 壳模型网格组件

class CShellMeshComponent :public CMeshComponent
{
public:
	CShellMeshComponent();

protected:
	void DrawQuadrilateral(FMeshRenderingData& MeshData, const fvector_4id& InDrawPoint, bool bReversal = false);

	
	fvector_4id GetQuadrilateralDrawPointTypeA(int InRowsand, int InColumns, int GroupLayer, int Offset = 1);
};
