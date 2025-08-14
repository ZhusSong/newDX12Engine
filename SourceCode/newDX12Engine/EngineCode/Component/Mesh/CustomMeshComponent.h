// 25.6.30 李
#pragma once
#include "Core/ShellMeshComponent.h"

// 外部资源类型
enum EMeshLoadAssetType
{
	MESH_OBJ,
	MESH_FBX,
};

class CCustomMeshComponent :public CMeshComponent
{
public:
	CCustomMeshComponent();

	void CreateMesh(FMeshRenderingData& MeshData, string& InPath);

	static bool LoadObjFromBuff(char* InBuff, uint32_t InBuffSize, FMeshRenderingData& MeshData);
	static bool LoadFBXFromBuff(const string& InPath, FMeshRenderingData& MeshData);


	void BuildKey(size_t& OutHashKey, std::string& InPath);
};
