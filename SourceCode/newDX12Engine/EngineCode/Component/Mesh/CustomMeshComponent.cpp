#include "CustomMeshComponent.h"
#include "../../Mesh/Core/MeshType.h"
#include "../../Mesh/Core/Material/Material.h"

#include <algorithm>
#include <map>
#include <unordered_map>

#if THIRD_PARTY_LIBRARY
#include "FBXSDK.h"

#pragma comment(lib, "DX12EngineModelTool.lib")

#endif // THIRD_PARTY_LIBRARY

std::map<size_t, std::vector<CCustomMeshComponent::FImportedMaterialSlot>> CCustomMeshComponent::ImportedMaterialSlotCache;

CCustomMeshComponent::CCustomMeshComponent()
{

}

#if THIRD_PARTY_LIBRARY
static std::string BuildTextureLookupKey(const std::string& InTexturePath)
{
	if (InTexturePath.empty())
	{
		return std::string();
	}

	std::string TexturePath = InTexturePath;
	std::replace(TexturePath.begin(), TexturePath.end(), '\\', '/');

	size_t FileNamePos = TexturePath.find_last_of('/');
	std::string FileName = (FileNamePos == std::string::npos) ? TexturePath : TexturePath.substr(FileNamePos + 1);

	size_t DotPos = FileName.find_last_of('.');
	if (DotPos != std::string::npos)
	{
		FileName = FileName.substr(0, DotPos);
	}

	return FileName;
}

static void ApplyImportedMaterial(CMaterial* InMaterial, const FFBXMaterial& InImportedMaterial)
{
	if (InMaterial == nullptr)
	{
		return;
	}

	const std::string BaseColorKey = BuildTextureLookupKey(InImportedMaterial.DiffuseMapFileName);
	if (!BaseColorKey.empty())
	{
		InMaterial->SetBaseColor(fvector_4d(1.f, 1.f, 1.f, 1.f));
		InMaterial->SetBaseColor(BaseColorKey);
		InMaterial->SetMaterialType(EMaterialType::BaseColor);
	}

	const std::string NormalKey = BuildTextureLookupKey(InImportedMaterial.BumpMapFileName);
	if (!NormalKey.empty())
	{
		InMaterial->SetNormal(NormalKey);
	}

	const std::string SpecularKey = BuildTextureLookupKey(InImportedMaterial.SpecularMapFileName);
	if (!SpecularKey.empty())
	{
		InMaterial->SetSpecular(SpecularKey);
	}
}

static void ApplyFallbackMaterial(CMaterial* InMaterial, const FFBXMaterial& InImportedMaterial)
{
	if (InMaterial == nullptr)
	{
		return;
	}

	const std::string BaseColorKey = BuildTextureLookupKey(InImportedMaterial.DiffuseMapFileName);
	if (!BaseColorKey.empty())
	{
		InMaterial->SetBaseColor(fvector_4d(1.f, 1.f, 1.f, 1.f));
		InMaterial->SetBaseColor(BaseColorKey);
		InMaterial->SetMaterialType(EMaterialType::BaseColor);
	}
}

static CCustomMeshComponent::FImportedMaterialSlot BuildImportedMaterialSlot(
	UINT InMaterialSlot,
	const FFBXMaterial& InImportedMaterial)
{
	CCustomMeshComponent::FImportedMaterialSlot ImportedMaterialSlot;
	ImportedMaterialSlot.MaterialSlotIndex = InMaterialSlot;
	ImportedMaterialSlot.BaseColorKey = BuildTextureLookupKey(InImportedMaterial.DiffuseMapFileName);
	ImportedMaterialSlot.NormalKey = std::string();
	ImportedMaterialSlot.SpecularKey = std::string();

	return ImportedMaterialSlot;
}

static void ApplyImportedMaterialSlot(
	CMaterial* InMaterial,
	const CCustomMeshComponent::FImportedMaterialSlot& InImportedMaterialSlot)
{
	if (InMaterial == nullptr)
	{
		return;
	}

	if (!InImportedMaterialSlot.BaseColorKey.empty())
	{
		InMaterial->SetBaseColor(fvector_4d(1.f, 1.f, 1.f, 1.f));
		InMaterial->SetBaseColor(InImportedMaterialSlot.BaseColorKey);
		InMaterial->SetMaterialType(EMaterialType::BaseColor);
	}

	if (!InImportedMaterialSlot.NormalKey.empty())
	{
		InMaterial->SetNormal(InImportedMaterialSlot.NormalKey);
	}

	if (!InImportedMaterialSlot.SpecularKey.empty())
	{
		InMaterial->SetSpecular(InImportedMaterialSlot.SpecularKey);
	}
}

static std::string ToLowerString(std::string InValue)
{
	for (char& Ch : InValue)
	{
		Ch = (char)tolower((unsigned char)Ch);
	}

	return InValue;
}

static bool EndsWith(const std::string& InValue, const std::string& InSuffix)
{
	if (InValue.size() < InSuffix.size())
	{
		return false;
	}

	return InValue.compare(InValue.size() - InSuffix.size(), InSuffix.size(), InSuffix) == 0;
}

static std::string StripKnownTextureSuffix(std::string InFileStem)
{
	const char* KnownSuffixes[] =
	{
		"_diff",
		"_albedo",
		"_basecolor",
		"_base_color",
		"_color",
		"_ddn",
		"_normal",
		"_norm",
		"_bump",
		"_spec",
		"_specular",
		"_gloss",
		"_rough",
		"_roughness",
		"_opacity",
		"_alpha",
		"_ao"
	};

	std::string LowerStem = ToLowerString(InFileStem);
	for (const char* Suffix : KnownSuffixes)
	{
		const std::string SuffixString = Suffix;
		if (EndsWith(LowerStem, SuffixString))
		{
			return InFileStem.substr(0, InFileStem.size() - SuffixString.size());
		}
	}

	return InFileStem;
}

static int GetFallbackMaterialPriority(const std::string& InGroupKey)
{
	const std::string LowerKey = ToLowerString(InGroupKey);
	if (LowerKey.find("main") != std::string::npos)
	{
		return 0;
	}

	if (LowerKey.find("decal") != std::string::npos)
	{
		return 1;
	}

	return 2;
}

static bool IsAmbientOcclusionTextureName(const std::string& InLowerFileStem)
{
	return InLowerFileStem.find("_ao") != std::string::npos ||
		InLowerFileStem.find("ambientocclusion") != std::string::npos ||
		InLowerFileStem.find("ambient_occlusion") != std::string::npos;
}

static std::vector<FFBXMaterial> BuildFallbackMaterialsFromFBXPath(const std::string& InPath)
{
	std::vector<FFBXMaterial> FallbackMaterials;

	char Filename[1024] = { 0 };
	get_path_clean_filename(Filename, InPath.c_str());

	std::string FileStem = Filename;
	size_t DotPos = FileStem.find_last_of('.');
	if (DotPos != std::string::npos)
	{
		FileStem = FileStem.substr(0, DotPos);
	}

	std::string FolderStem = FileStem;
	std::string LowerFolderStem = ToLowerString(FolderStem);
	size_t MarkPos = LowerFolderStem.find("_mark_");
	if (MarkPos != std::string::npos)
	{
		FolderStem = FolderStem.substr(0, MarkPos);
	}

	const std::string TextureRoot =
		FEnginePathHelper::RelativeToAbsolutePath(
			FEnginePathHelper::GetEngineAssetPath()) + "/Texture/" + FolderStem;

	def_c_paths Paths;
	init_def_c_paths(&Paths);
	find_files(TextureRoot.c_str(), &Paths, true);

	std::map<std::string, FFBXMaterial> MaterialGroups;
	for (int i = 0; i < Paths.index; i++)
	{
		if (find_string(Paths.paths[i], ".png", 0) == -1 &&
			find_string(Paths.paths[i], ".PNG", 0) == -1 &&
			find_string(Paths.paths[i], ".jpg", 0) == -1 &&
			find_string(Paths.paths[i], ".JPG", 0) == -1 &&
			find_string(Paths.paths[i], ".jpeg", 0) == -1 &&
			find_string(Paths.paths[i], ".JPEG", 0) == -1 &&
			find_string(Paths.paths[i], ".dds", 0) == -1 &&
			find_string(Paths.paths[i], ".DDS", 0) == -1)
		{
			continue;
		}

		char CleanFilename[1024] = { 0 };
		get_path_clean_filename(CleanFilename, Paths.paths[i]);

		std::string FileName = CleanFilename;
		size_t FileDotPos = FileName.find_last_of('.');
		std::string FileStemOnly = (FileDotPos == std::string::npos) ? FileName : FileName.substr(0, FileDotPos);
		std::string LowerFileStem = ToLowerString(FileStemOnly);
		std::string GroupKey = StripKnownTextureSuffix(FileStemOnly);

		FFBXMaterial& GroupMaterial = MaterialGroups[GroupKey];
		if (LowerFileStem.find("diff") != std::string::npos ||
			LowerFileStem.find("albedo") != std::string::npos ||
			LowerFileStem.find("basecolor") != std::string::npos ||
			LowerFileStem.find("base_color") != std::string::npos ||
			LowerFileStem.find("color") != std::string::npos)
		{
			GroupMaterial.DiffuseMapFileName = Paths.paths[i];
		}
		else if (LowerFileStem.find("ddn") != std::string::npos ||
			LowerFileStem.find("normal") != std::string::npos ||
			LowerFileStem.find("bump") != std::string::npos)
		{
			GroupMaterial.BumpMapFileName = Paths.paths[i];
		}
		else if (LowerFileStem.find("spec") != std::string::npos ||
			LowerFileStem.find("gloss") != std::string::npos ||
			LowerFileStem.find("rough") != std::string::npos)
		{
			GroupMaterial.SpecularMapFileName = Paths.paths[i];
		}
		else if (LowerFileStem.find("opacity") != std::string::npos ||
			LowerFileStem.find("alpha") != std::string::npos)
		{
			GroupMaterial.AlphaMapFileName = Paths.paths[i];
		}
		else if (!IsAmbientOcclusionTextureName(LowerFileStem) &&
			GroupMaterial.DiffuseMapFileName.empty())
		{
			GroupMaterial.DiffuseMapFileName = Paths.paths[i];
		}
	}

	auto BaseMaterialGroup = MaterialGroups.find(FolderStem);
	for (auto& MaterialGroup : MaterialGroups)
	{
		if (MaterialGroup.second.DiffuseMapFileName.empty() &&
			BaseMaterialGroup != MaterialGroups.end() &&
			!BaseMaterialGroup->second.DiffuseMapFileName.empty())
		{
			MaterialGroup.second.DiffuseMapFileName = BaseMaterialGroup->second.DiffuseMapFileName;
		}
	}

	std::vector<std::pair<std::string, FFBXMaterial>> SortedGroups(MaterialGroups.begin(), MaterialGroups.end());
	std::sort(
		SortedGroups.begin(),
		SortedGroups.end(),
		[](const std::pair<std::string, FFBXMaterial>& A, const std::pair<std::string, FFBXMaterial>& B)
		{
			const int PriorityA = GetFallbackMaterialPriority(A.first);
			const int PriorityB = GetFallbackMaterialPriority(B.first);
			if (PriorityA != PriorityB)
			{
				return PriorityA < PriorityB;
			}

			return A.first < B.first;
		});

	for (const auto& MaterialGroup : SortedGroups)
	{
		if (!MaterialGroup.second.DiffuseMapFileName.empty() ||
			!MaterialGroup.second.BumpMapFileName.empty() ||
			!MaterialGroup.second.SpecularMapFileName.empty() ||
			!MaterialGroup.second.AlphaMapFileName.empty())
		{
			FallbackMaterials.push_back(MaterialGroup.second);
		}
	}

	return FallbackMaterials;
}
#endif

void CCustomMeshComponent::CreateMesh(FMeshRenderingData& MeshData, string& InPath)
{
	// 获取外部文件路径
	// 外部ファイルパスを取得
	// x.obj
	char Buff[1024] = { 0 };
	get_path_clean_filename(Buff, InPath.c_str());
	if (find_string(Buff, ".obj", 0) != -1 ||
		find_string(Buff, ".OBJ", 0) != -1)
	{
		// 拿到文件大小
		// ファイルサイズを取得
		unsigned int FileSize = get_file_size_by_filename(InPath.c_str());

		// 根据文件大小创建buff
		// ファイルサイズに基づいてバッファを作成
		char* Buff = new char[FileSize + 1];
		// 必须要初始化
		// 初期化が必要
		memset(Buff, 0, FileSize + 1);

		// 提取buff
		// バッファを取得
		get_file_buf(InPath.c_str(), Buff);

		if (!LoadObjFromBuff(Buff, FileSize, MeshData))
		{

		}

		delete Buff;
	}
	else if(find_string(Buff, ".fbx", 0) != -1 ||
		find_string(Buff, ".FBX", 0) != -1)
	{
		char PathBuff[1024] = { 0 };
		get_full_path(PathBuff, 1024, InPath.c_str());

		LoadFBXFromBuff(PathBuff, MeshData);

		size_t HashKey = 0;
		BuildKey(HashKey, InPath);
		ImportedMaterialSlotCache[HashKey] = ImportedMaterialSlots;
	}
}

bool CCustomMeshComponent::LoadObjFromBuff(char* InBuff, uint32_t InBuffSize, FMeshRenderingData& MeshData)
{
	if (InBuffSize > 0)
	{
		stringstream BuffStream(InBuff);
		char TmpLine[256] = { 0 };
		string MidTmpTag;

		for (; !BuffStream.eof();)
		{
			memset(TmpLine, 0, 256);

			// 读取一行数据
			// 1行分のデータを読み込む
			BuffStream.getline(TmpLine, 256);
			if (strlen(TmpLine) > 0)
			{
				if (TmpLine[0] == 'v')
				{
					stringstream LineStream(TmpLine);
					LineStream >> MidTmpTag;

					if (TmpLine[1] == 'n')
					{
					}
					else if (TmpLine[1] == 't')
					{
					}
					else
					{
						// 先添加一个
						// まず1つ追加
						MeshData.VertexData.push_back(FVertex(
							XMFLOAT3(), XMFLOAT4(Colors::White)));

						// 拿到添加后的位置
						// 追加後の位置を取得
						int TopIndex = MeshData.VertexData.size() - 1;
						XMFLOAT3& Float3InPos = MeshData.VertexData[TopIndex].Position;

						// 解析了位置
						// 位置を解析
						LineStream >> Float3InPos.x;
						LineStream >> Float3InPos.y;
						LineStream >> Float3InPos.z;
					}
				}
				else if (TmpLine[0] == 'f')
				{
					stringstream LineStream(TmpLine);
					LineStream >> MidTmpTag;

					char SaveLineString[256] = { 0 };
					char TmpBuff[256] = { 0 };
					for (size_t i = 0; i < 3; i++)
					{
						memset(SaveLineString, 0, 256);
						// 输入一行数据
						// 1行のデータを入力
						LineStream >> SaveLineString;

						// 找到索引的位置
						// インデックスの位置を見つける
						int StringPosA = find_string(SaveLineString, "/", 0);
						memset(TmpBuff, 0, 256);
						char* VPosIndex = string_mid(SaveLineString, TmpBuff, 0, StringPosA);

						// 放到索引容器里面
						// インデックスコンテナに格納
						MeshData.IndexData.push_back(atoi(VPosIndex) - 1);

						// 纹理Index
						// テクスチャインデックス
						int StringPosB = find_string(SaveLineString, "/", StringPosA + 1);
						memset(TmpBuff, 0, 256);
						char* TexcoordIndex = string_mid(SaveLineString, TmpBuff, StringPosA + 1, StringPosB - (StringPosA + 1));

						// 法线index
						// 法線インデックス
						memset(TmpBuff, 0, 256);
						char* NormalIndex = string_mid(SaveLineString, TmpBuff, StringPosB + 1, strlen(SaveLineString) - (StringPosB + 1));
					}
				}
			}
		}

		return true;
	}

	return false;
}

void CCustomMeshComponent::BuildKey(size_t& OutHashKey, std::string& InPath)
{
	std::hash<string> FloatHash;

	OutHashKey = 3;
	OutHashKey += FloatHash(InPath);
}

void CCustomMeshComponent::ApplyCachedMeshMetadata(const size_t InMeshHash)
{
#if THIRD_PARTY_LIBRARY
	auto FindImportedMaterials = ImportedMaterialSlotCache.find(InMeshHash);
	if (FindImportedMaterials == ImportedMaterialSlotCache.end())
	{
		return;
	}

	ImportedMaterialSlots = FindImportedMaterials->second;
	if (ImportedMaterialSlots.empty())
	{
		return;
	}

	SetMaterialCount((UINT)ImportedMaterialSlots.size());

	for (const FImportedMaterialSlot& ImportedMaterialSlot : ImportedMaterialSlots)
	{
		ApplyImportedMaterialSlot(
			GetMaterialBySlot(ImportedMaterialSlot.MaterialSlotIndex),
			ImportedMaterialSlot);
	}
#endif
}

bool CCustomMeshComponent::LoadFBXFromBuff(const string& InPath, FMeshRenderingData& MeshData)
{
#if THIRD_PARTY_LIBRARY
	FFBXRenderData RenderData;
	FFBXAssetImport().LoadMeshData(InPath.c_str(), RenderData);

	ImportedMaterialSlots.clear();
	const std::vector<FFBXMaterial> FallbackMaterials = BuildFallbackMaterialsFromFBXPath(InPath);

	UINT NextMaterialSlot = 0;

	for (auto& TmpModel : RenderData.ModelData)
	{
		std::unordered_map<int, UINT> MaterialSlotMapping;

		for (auto& MeshTmp : TmpModel.MeshData)
		{
			FMeshSection Section;
			Section.VertexOffset = (UINT)MeshData.VertexData.size();
			Section.IndexOffset = (UINT)MeshData.IndexData.size();

			for (auto& VertexTmp : MeshTmp.VertexData)
			{
				for (int i = 0; i < 3; i++)
				{
					MeshData.VertexData.push_back(FVertex());
					FVertex& InVertex = MeshData.VertexData[MeshData.VertexData.size() - 1];

					InVertex.Position.x = VertexTmp.Vertexs[i].Position.X;
					InVertex.Position.y = VertexTmp.Vertexs[i].Position.Y;
					InVertex.Position.z = VertexTmp.Vertexs[i].Position.Z;

					InVertex.Normal.x = VertexTmp.Vertexs[i].Normal.X;
					InVertex.Normal.y = VertexTmp.Vertexs[i].Normal.Y;
					InVertex.Normal.z = VertexTmp.Vertexs[i].Normal.Z;

					InVertex.UTangent.x = VertexTmp.Vertexs[i].Tangent.X;
					InVertex.UTangent.y = VertexTmp.Vertexs[i].Tangent.Y;
					InVertex.UTangent.z = VertexTmp.Vertexs[i].Tangent.Z;

					InVertex.TexCoord.x = VertexTmp.Vertexs[i].UV.X;
					InVertex.TexCoord.y = VertexTmp.Vertexs[i].UV.Y;
				}
			}

			for (uint16_t IndexValue : MeshTmp.IndexData)
			{
				MeshData.IndexData.push_back(IndexValue);
			}

			Section.VertexSize = (UINT)MeshData.VertexData.size() - Section.VertexOffset;
			Section.IndexSize = (UINT)MeshData.IndexData.size() - Section.IndexOffset;

			UINT MaterialSlot = 0;
			auto FindMaterialSlot = MaterialSlotMapping.find(MeshTmp.MaterialID);
			if (FindMaterialSlot == MaterialSlotMapping.end())
			{
				MaterialSlot = NextMaterialSlot++;
				MaterialSlotMapping.emplace(MeshTmp.MaterialID, MaterialSlot);

				SetMaterialCount(NextMaterialSlot);
				auto MaterialIterator = TmpModel.MaterialMap.find(MeshTmp.MaterialID);
				if (MaterialIterator != TmpModel.MaterialMap.end())
				{
					ApplyImportedMaterial(GetMaterialBySlot(MaterialSlot), MaterialIterator->second);
					ImportedMaterialSlots.push_back(
						BuildImportedMaterialSlot(MaterialSlot, MaterialIterator->second));
				}
				else
				{
					FImportedMaterialSlot ImportedMaterialSlot;
					ImportedMaterialSlot.MaterialSlotIndex = MaterialSlot;

					if (!FallbackMaterials.empty())
					{
						const FFBXMaterial& FallbackMaterial =
							FallbackMaterials[std::min<size_t>(MaterialSlot, FallbackMaterials.size() - 1)];

						ApplyFallbackMaterial(GetMaterialBySlot(MaterialSlot), FallbackMaterial);
						ImportedMaterialSlot = BuildImportedMaterialSlot(MaterialSlot, FallbackMaterial);
						ImportedMaterialSlots.push_back(ImportedMaterialSlot);
					}
					else
					{
						ImportedMaterialSlots.push_back(ImportedMaterialSlot);
					}
				}
			}
			else
			{
				MaterialSlot = FindMaterialSlot->second;
			}

			Section.MaterialSlotIndex = MaterialSlot;
			MeshData.Sections.push_back(Section);
		}
	}
#endif
	return true;
}
