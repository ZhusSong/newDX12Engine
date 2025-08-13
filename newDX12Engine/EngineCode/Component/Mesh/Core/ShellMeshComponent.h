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

	
	// 绘制拥有row和colum的模型
	// 比如在输入点的时候先绘制一圈row 到下一层再绘制一圈row
	// GroupLayer 绘制一圈row的数量是多少
	// Offset 是否在绘制中有间隔式的绘制
	fvector_4id GetQuadrilateralDrawPointTypeA(int InRowsand, int InColumns, int GroupLayer, int Offset = 1);
};
