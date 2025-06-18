// 25.5.16 李
#pragma once
#include "Core/ShellMeshComponent.h"

class CCustomMeshComponent :public CMeshComponent
{
public:
	CCustomMeshComponent();

	void CreateMesh(FMeshRenderingData& MeshData, string& InPath);

	static bool LoadObjFromBuff(char* InBuff, uint32_t InBuffSize, FMeshRenderingData& MeshData);

	void BuildKey(size_t& OutHashKey, std::string& InPath);
};
