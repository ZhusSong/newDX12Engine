#pragma once
#include "Core/ShellMeshComponent.h"
#include "CustomMeshComponent.CodeReflection.h"

#include <map>
#include <string>
#include <vector>

// 外部资源类型
// 外部リソースの種類
enum EMeshLoadAssetType
{
	MESH_OBJ,
	MESH_FBX,
};

class CCustomMeshComponent :public CMeshComponent
{
	CODEREFLECTION()
public:
	CCustomMeshComponent();

	void CreateMesh(FMeshRenderingData& MeshData, string& InPath);
	void ApplyCachedMeshMetadata(const size_t InMeshHash) override;

	static bool LoadObjFromBuff(char* InBuff, uint32_t InBuffSize, FMeshRenderingData& MeshData);
	bool LoadFBXFromBuff(const string& InPath, FMeshRenderingData& MeshData);


	void BuildKey(size_t& OutHashKey, std::string& InPath);
	struct FImportedMaterialSlot
	{
		UINT MaterialSlotIndex = 0;
		std::string BaseColorKey;
		std::string NormalKey;
		std::string SpecularKey;
	};

	std::vector<FImportedMaterialSlot> ImportedMaterialSlots;

	static std::map<size_t, std::vector<FImportedMaterialSlot>> ImportedMaterialSlotCache;
};
