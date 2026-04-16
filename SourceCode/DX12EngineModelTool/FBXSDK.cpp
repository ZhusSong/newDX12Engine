#include "ThirdPartyLibrary/FBX/include/fbxsdk.h"
#include "FBXSDK.h"

#ifdef _DEBUG
#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")
#else
#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")
#endif

#include <sys/stat.h>   
#include <cstring>     
#include <set>
#include <numeric>      
#include <string>

#include "miniz.h"

void InitializeSdkObjects(FbxManager*& InManager, FbxScene*& InScene)
{
	InManager = FbxManager::Create();

	FbxIOSettings* FBXIO = FbxIOSettings::Create(InManager, IOSROOT);
	InManager->SetIOSettings(FBXIO);

	FbxString FBXPath = FbxGetApplicationDirectory();
	InManager->LoadPluginsDirectory(FBXPath);

	InScene = FbxScene::Create(InManager, "Create FBX ");
}


bool LoadScene(FbxManager* InManager, FbxDocument* InScene, const char* InFilename)
{
	FFBXVersion SDKVersion;
	FbxManager::GetFileFormatVersion(
		SDKVersion.Major,
		SDKVersion.Minor,
		SDKVersion.Revision);

	FbxImporter* FBXImporterPtr = FbxImporter::Create(InManager, "");

	bool bReturn = FBXImporterPtr->Initialize(
		InFilename,
		-1,
		InManager->GetIOSettings());

	FFBXVersion FileVersion;
	FBXImporterPtr->GetFileVersion(
		FileVersion.Major,
		FileVersion.Minor,
		FileVersion.Revision);

	if (!bReturn)
	{
		return false;
	}

	if (FBXImporterPtr->IsFBX())
	{

	}

	bReturn = FBXImporterPtr->Import(InScene);
	if (bReturn &&
		FBXImporterPtr->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{

	}


	FBXImporterPtr->Destroy();

	return true;
}


void GetPolygons(FbxMesh* InMesh, FFBXMesh& OutData)
{
	int PolygonCount = InMesh->GetPolygonCount();
	FbxVector4* ControlPoints = InMesh->GetControlPoints();
	FbxGeometryElementMaterial* MaterialElement = InMesh->GetElementMaterial();

	int VertexID = 0;
	for (int i = 0; i < PolygonCount; i++)//Get Polygon
	{
		OutData.VertexData.push_back(FFBXTriangle());
		FFBXTriangle& InTriangle = OutData.VertexData[OutData.VertexData.size() - 1];

		if (MaterialElement)
		{
			switch (MaterialElement->GetMappingMode())
			{
			case FbxGeometryElement::eByPolygon:
				if (MaterialElement->GetIndexArray().GetCount() > i)
				{
					InTriangle.MaterialID = MaterialElement->GetIndexArray().GetAt(i);
				}
				break;
			case FbxGeometryElement::eAllSame:
				if (MaterialElement->GetIndexArray().GetCount() > 0)
				{
					InTriangle.MaterialID = MaterialElement->GetIndexArray().GetAt(0);
				}
				break;
			}
		}

		int PolygonSize = InMesh->GetPolygonSize(i);
		for (int j = 0; j < PolygonSize; j++)
		{
			int ControlPointIndex = InMesh->GetPolygonVertex(i, j);

			// Coordinates
			// FbxVector4 Coordinates;
			// Get Pos
			{
				// 缩放
				// スケーリング
				FbxDouble3 Scaling = InMesh->GetNode()->LclScaling;

				InTriangle.Vertexs[j].Position.X = ControlPoints[ControlPointIndex].mData[0] * Scaling[0];
				InTriangle.Vertexs[j].Position.Y = ControlPoints[ControlPointIndex].mData[2] * Scaling[2];
				InTriangle.Vertexs[j].Position.Z = -ControlPoints[ControlPointIndex].mData[1] * Scaling[1];
			}

			// 顶点颜色
			// 頂点カラー
			for (int l = 0; l < InMesh->GetElementVertexColorCount(); l++) {}

			//UV
			for (int l = 0; l < InMesh->GetElementUVCount(); ++l)
			{
				FbxGeometryElementUV* TextureUV = InMesh->GetElementUV(l);
				auto ModeType = TextureUV->GetMappingMode();

				auto ReferenceMode = TextureUV->GetReferenceMode();
				if (ModeType == fbxsdk::FbxLayerElement::eByControlPoint)
				{
					if (ReferenceMode == fbxsdk::FbxLayerElement::eDirect)
					{
						FbxVector2 UV = TextureUV->GetDirectArray().GetAt(ControlPointIndex);

						InTriangle.Vertexs[j].UV.X = UV.mData[0];
						InTriangle.Vertexs[j].UV.Y = 1.f - UV.mData[1];//UV取反 //UVを反転
					}
					else if (ReferenceMode == fbxsdk::FbxLayerElement::eIndexToDirect)
					{
						int ID = TextureUV->GetIndexArray().GetAt(ControlPointIndex);

						FbxVector2 UV = TextureUV->GetDirectArray().GetAt(ID);
						InTriangle.Vertexs[j].UV.X = UV.mData[0];
						InTriangle.Vertexs[j].UV.Y = 1.f - UV.mData[1];//UV取反 //UVを反転
					}
				}
				else if (ModeType == fbxsdk::FbxLayerElement::eByPolygonVertex)
				{
					FbxString UVSetName = TextureUV->GetName();
					if (UVSetName.IsEmpty())
					{
						FbxStringList UVSetNames;
						InMesh->GetUVSetNames(UVSetNames);
						if (UVSetNames.GetCount() > l)
						{
							UVSetName = UVSetNames[l];
						}
						else if (UVSetNames.GetCount() > 0)
						{
							UVSetName = UVSetNames[0];
						}
					}

					bool bUnmapped = false;
					FbxVector2 UV;
					bool bHasPolygonVertexUV = false;
					if (!UVSetName.IsEmpty())
					{
						bHasPolygonVertexUV = InMesh->GetPolygonVertexUV(i, j, UVSetName.Buffer(), UV, bUnmapped) && !bUnmapped;
					}

					if (!bHasPolygonVertexUV)
					{
						int TextureUVIndex = InMesh->GetTextureUVIndex(i, j);
						switch (ReferenceMode)
						{
						case fbxsdk::FbxLayerElement::eDirect:
						{
							UV = TextureUV->GetDirectArray().GetAt(TextureUVIndex);
							bHasPolygonVertexUV = true;
							break;
						}
						case fbxsdk::FbxLayerElement::eIndexToDirect:
						{
							int ID = TextureUV->GetIndexArray().GetAt(TextureUVIndex);
							UV = TextureUV->GetDirectArray().GetAt(ID);
							bHasPolygonVertexUV = true;
							break;
						}
						}
					}

					if (bHasPolygonVertexUV)
					{
						InTriangle.Vertexs[j].UV.X = UV.mData[0];
						InTriangle.Vertexs[j].UV.Y = 1.f - UV.mData[1];//UV取反 //UVを反転
					}
				}
			}

			// 法線
			for (int l = 0; l < InMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* Normal = InMesh->GetElementNormal(l);
				auto NormalReferenceMode = Normal->GetReferenceMode();

				if (Normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (NormalReferenceMode)
					{
					case fbxsdk::FbxLayerElement::eDirect:
					{
						FbxVector4 NormalPoint = Normal->GetDirectArray().GetAt(VertexID);

						InTriangle.Vertexs[j].Normal.X = NormalPoint.mData[0];
						InTriangle.Vertexs[j].Normal.Y = NormalPoint.mData[2];
						InTriangle.Vertexs[j].Normal.Z = NormalPoint.mData[1];

						break;
					}
					case fbxsdk::FbxLayerElement::eIndexToDirect:
					{
						int ID = Normal->GetIndexArray().GetAt(VertexID);

						FbxVector4 NormalPoint = Normal->GetDirectArray().GetAt(ID);
						InTriangle.Vertexs[j].Normal.X = NormalPoint.mData[0];
						InTriangle.Vertexs[j].Normal.Y = NormalPoint.mData[2];
						InTriangle.Vertexs[j].Normal.Z = NormalPoint.mData[1];
					}
					}
				}
				else if (Normal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					switch (NormalReferenceMode)
					{
					case fbxsdk::FbxLayerElement::eDirect:
					{
						FbxVector4 NormalPoint = Normal->GetDirectArray().GetAt(ControlPointIndex);
						InTriangle.Vertexs[j].Normal.X = NormalPoint.mData[0];
						InTriangle.Vertexs[j].Normal.Y = NormalPoint.mData[2];
						InTriangle.Vertexs[j].Normal.Z = NormalPoint.mData[1];
						break;
					}
					case fbxsdk::FbxLayerElement::eIndexToDirect:
					{
						int ID = Normal->GetIndexArray().GetAt(ControlPointIndex);
						FbxVector4 NormalPoint = Normal->GetDirectArray().GetAt(ID);
						InTriangle.Vertexs[j].Normal.X = NormalPoint.mData[0];
						InTriangle.Vertexs[j].Normal.Y = NormalPoint.mData[2];
						InTriangle.Vertexs[j].Normal.Z = NormalPoint.mData[1];
						break;
					}
					}
				}
			}

			//切线
			//接線
			for (int l = 0; l < InMesh->GetElementTangentCount(); ++l)
			{
				FbxGeometryElementTangent* Tangent = InMesh->GetElementTangent(l);

				if (Tangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (Tangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						FbxVector4 TangentValue = Tangent->GetDirectArray().GetAt(VertexID);

						InTriangle.Vertexs[j].Tangent.X = TangentValue.mData[0];
						InTriangle.Vertexs[j].Tangent.Y = TangentValue.mData[2];
						InTriangle.Vertexs[j].Tangent.Z = TangentValue.mData[1];
						break;
					}
					case FbxGeometryElement::eIndexToDirect:
					{
						int ID = Tangent->GetIndexArray().GetAt(VertexID);

						FbxVector4 TangentValue = Tangent->GetDirectArray().GetAt(ID);
						InTriangle.Vertexs[j].Tangent.X = TangentValue.mData[0];
						InTriangle.Vertexs[j].Tangent.Y = TangentValue.mData[2];
						InTriangle.Vertexs[j].Tangent.Z = TangentValue.mData[1];
						break;
					}
					}
				}
			}

			// 副法线
			// 従法線
			for (int l = 0; l < InMesh->GetElementBinormalCount(); ++l)
			{
				FbxGeometryElementBinormal* Binormal = InMesh->GetElementBinormal(l);

				if (Binormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (Binormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						Binormal->GetDirectArray().GetAt(VertexID);
						break;
					}
					}
				}
			}

			VertexID++;
		}
	}
}

static bool ReadTexturePathFromProperty(FbxProperty InProperty, std::string& OutPath)
{
	if (!InProperty.IsValid())
	{
		return false;
	}

	const int LayeredTextureCount = InProperty.GetSrcObjectCount<FbxLayeredTexture>();
	for (int LayeredTextureIndex = 0; LayeredTextureIndex < LayeredTextureCount; ++LayeredTextureIndex)
	{
		if (FbxLayeredTexture* LayeredTexture = InProperty.GetSrcObject<FbxLayeredTexture>(LayeredTextureIndex))
		{
			const int FileTextureCount = LayeredTexture->GetSrcObjectCount<FbxFileTexture>();
			for (int TextureIndex = 0; TextureIndex < FileTextureCount; ++TextureIndex)
			{
				if (FbxFileTexture* FileTexture = LayeredTexture->GetSrcObject<FbxFileTexture>(TextureIndex))
				{
					const char* RelativeFileName = FileTexture->GetRelativeFileName();
					if (RelativeFileName && RelativeFileName[0] != '\0')
					{
						OutPath = RelativeFileName;
						return true;
					}

					const char* FileName = FileTexture->GetFileName();
					if (FileName && FileName[0] != '\0')
					{
						OutPath = FileName;
						return true;
					}
				}
			}
		}
	}

	const int FileTextureCount = InProperty.GetSrcObjectCount<FbxFileTexture>();
	for (int TextureIndex = 0; TextureIndex < FileTextureCount; ++TextureIndex)
	{
		if (FbxFileTexture* FileTexture = InProperty.GetSrcObject<FbxFileTexture>(TextureIndex))
		{
			const char* RelativeFileName = FileTexture->GetRelativeFileName();
			if (RelativeFileName && RelativeFileName[0] != '\0')
			{
				OutPath = RelativeFileName;
				return true;
			}

			const char* FileName = FileTexture->GetFileName();
			if (FileName && FileName[0] != '\0')
			{
				OutPath = FileName;
				return true;
			}
		}
	}

	return false;
}

static bool ReadSurfaceMaterial(FbxSurfaceMaterial* InMaterial, FFBXMaterial& OutMaterial)
{
	if (InMaterial == nullptr)
	{
		return false;
	}

	bool bHasAnyTexture = false;
	bHasAnyTexture |= ReadTexturePathFromProperty(InMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse), OutMaterial.DiffuseMapFileName);
	bHasAnyTexture |= ReadTexturePathFromProperty(InMaterial->FindProperty(FbxSurfaceMaterial::sSpecular), OutMaterial.SpecularMapFileName);
	bHasAnyTexture |= ReadTexturePathFromProperty(InMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor), OutMaterial.AlphaMapFileName);
	bHasAnyTexture |= ReadTexturePathFromProperty(InMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap), OutMaterial.BumpMapFileName);

	if (OutMaterial.BumpMapFileName.empty())
	{
		bHasAnyTexture |= ReadTexturePathFromProperty(InMaterial->FindProperty(FbxSurfaceMaterial::sBump), OutMaterial.BumpMapFileName);
	}

	if (!bHasAnyTexture)
	{
		for (FbxProperty Property = InMaterial->GetFirstProperty();
			Property.IsValid();
			Property = InMaterial->GetNextProperty(Property))
		{
			std::string TexturePath;
			if (!ReadTexturePathFromProperty(Property, TexturePath))
			{
				continue;
			}

			std::string PropertyName = Property.GetNameAsCStr() ? Property.GetNameAsCStr() : "";
			std::string LowerPropertyName = PropertyName;
			for (char& Ch : LowerPropertyName)
			{
				Ch = (char)tolower((unsigned char)Ch);
			}

			if (OutMaterial.DiffuseMapFileName.empty() &&
				(LowerPropertyName.find("diffuse") != std::string::npos ||
				 LowerPropertyName.find("base") != std::string::npos ||
				 LowerPropertyName.find("albedo") != std::string::npos ||
				 LowerPropertyName.find("color") != std::string::npos))
			{
				OutMaterial.DiffuseMapFileName = TexturePath;
				bHasAnyTexture = true;
				continue;
			}

			if (OutMaterial.BumpMapFileName.empty() &&
				(LowerPropertyName.find("normal") != std::string::npos ||
				 LowerPropertyName.find("bump") != std::string::npos ||
				 LowerPropertyName.find("ddn") != std::string::npos))
			{
				OutMaterial.BumpMapFileName = TexturePath;
				bHasAnyTexture = true;
				continue;
			}

			if (OutMaterial.SpecularMapFileName.empty() &&
				(LowerPropertyName.find("spec") != std::string::npos ||
				 LowerPropertyName.find("gloss") != std::string::npos ||
				 LowerPropertyName.find("rough") != std::string::npos))
			{
				OutMaterial.SpecularMapFileName = TexturePath;
				bHasAnyTexture = true;
				continue;
			}

			if (OutMaterial.AlphaMapFileName.empty() &&
				(LowerPropertyName.find("alpha") != std::string::npos ||
				 LowerPropertyName.find("opacity") != std::string::npos ||
				 LowerPropertyName.find("transparency") != std::string::npos))
			{
				OutMaterial.AlphaMapFileName = TexturePath;
				bHasAnyTexture = true;
				continue;
			}

			if (OutMaterial.DiffuseMapFileName.empty())
			{
				OutMaterial.DiffuseMapFileName = TexturePath;
				bHasAnyTexture = true;
			}
		}
	}

	if (!bHasAnyTexture)
	{
		const int FileTextureCount = InMaterial->GetSrcObjectCount<FbxFileTexture>();
		for (int TextureIndex = 0; TextureIndex < FileTextureCount; ++TextureIndex)
		{
			if (FbxFileTexture* FileTexture = InMaterial->GetSrcObject<FbxFileTexture>(TextureIndex))
			{
				std::string TexturePath;

				const char* RelativeFileName = FileTexture->GetRelativeFileName();
				if (RelativeFileName && RelativeFileName[0] != '\0')
				{
					TexturePath = RelativeFileName;
				}
				else
				{
					const char* FileName = FileTexture->GetFileName();
					if (FileName && FileName[0] != '\0')
					{
						TexturePath = FileName;
					}
				}

				if (TexturePath.empty())
				{
					continue;
				}

				std::string TextureName = FileTexture->GetName();
				std::string LowerTextureName = TextureName;
				for (char& Ch : LowerTextureName)
				{
					Ch = (char)tolower((unsigned char)Ch);
				}

				if (OutMaterial.DiffuseMapFileName.empty() &&
					(LowerTextureName.find("diff") != std::string::npos ||
					 LowerTextureName.find("base") != std::string::npos ||
					 LowerTextureName.find("albedo") != std::string::npos))
				{
					OutMaterial.DiffuseMapFileName = TexturePath;
					bHasAnyTexture = true;
					continue;
				}

				if (OutMaterial.BumpMapFileName.empty() &&
					(LowerTextureName.find("normal") != std::string::npos ||
					 LowerTextureName.find("bump") != std::string::npos ||
					 LowerTextureName.find("ddn") != std::string::npos))
				{
					OutMaterial.BumpMapFileName = TexturePath;
					bHasAnyTexture = true;
					continue;
				}

				if (OutMaterial.SpecularMapFileName.empty() &&
					(LowerTextureName.find("spec") != std::string::npos ||
					 LowerTextureName.find("gloss") != std::string::npos ||
					 LowerTextureName.find("rough") != std::string::npos))
				{
					OutMaterial.SpecularMapFileName = TexturePath;
					bHasAnyTexture = true;
					continue;
				}

				if (OutMaterial.AlphaMapFileName.empty() &&
					(LowerTextureName.find("alpha") != std::string::npos ||
					 LowerTextureName.find("opacity") != std::string::npos ||
					 LowerTextureName.find("transparency") != std::string::npos))
				{
					OutMaterial.AlphaMapFileName = TexturePath;
					bHasAnyTexture = true;
					continue;
				}

				if (OutMaterial.DiffuseMapFileName.empty())
				{
					OutMaterial.DiffuseMapFileName = TexturePath;
					bHasAnyTexture = true;
				}
			}
		}
	}

	return bHasAnyTexture;
}

static void GetMaterial(FbxNode* InNode, const std::set<int>& InMaterialIndices, FFBXModel& InModel)
{
	if (InNode == nullptr)
	{
		return;
	}

	const int MaterialCount = InNode->GetMaterialCount();
	for (int MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		if (FbxSurfaceMaterial* SurfaceMaterial = InNode->GetMaterial(MaterialIndex))
		{
			FFBXMaterial MaterialData;
			if (ReadSurfaceMaterial(SurfaceMaterial, MaterialData))
			{
				InModel.MaterialMap[MaterialIndex] = std::move(MaterialData);
			}
		}
	}

	const int ConnectedMaterialCount = InNode->GetSrcObjectCount<FbxSurfaceMaterial>();
	for (int MaterialIndex = 0; MaterialIndex < ConnectedMaterialCount; ++MaterialIndex)
	{
		if (FbxSurfaceMaterial* SurfaceMaterial = InNode->GetSrcObject<FbxSurfaceMaterial>(MaterialIndex))
		{
			FFBXMaterial MaterialData;
			if (ReadSurfaceMaterial(SurfaceMaterial, MaterialData))
			{
				InModel.MaterialMap[MaterialIndex] = std::move(MaterialData);
			}
		}
	}

	if (FbxScene* Scene = InNode->GetScene())
	{
		const int SceneMaterialCount = Scene->GetMaterialCount();
		for (int MaterialIndex : InMaterialIndices)
		{
			if (MaterialIndex < 0 || MaterialIndex >= SceneMaterialCount)
			{
				continue;
			}

			if (InModel.MaterialMap.find(MaterialIndex) != InModel.MaterialMap.end())
			{
				continue;
			}

			if (FbxSurfaceMaterial* SurfaceMaterial = Scene->GetMaterial(MaterialIndex))
			{
				FFBXMaterial MaterialData;
				if (ReadSurfaceMaterial(SurfaceMaterial, MaterialData))
				{
					InModel.MaterialMap[MaterialIndex] = std::move(MaterialData);
				}
			}
		}
	}
}

void GetIndex(FFBXMesh& InMesh)
{
	uint16_t VertexDataSize = InMesh.VertexData.size() * 3.f;
	InMesh.IndexData.resize(InMesh.VertexData.size() * 3.f);

	for (uint16_t i = 0; i < VertexDataSize; i++)
	{
		InMesh.IndexData[i] = i;
	}
}
void GetMesh(FbxNode* InNode, FFBXModel& InModel)
{
	FbxMesh* NodeMesh = (FbxMesh*)InNode->GetNodeAttribute();

	FFBXMesh RawMesh;
	GetPolygons(NodeMesh, RawMesh);

	std::set<int> MaterialIndices;
	std::map<int, FFBXMesh> MeshByMaterial;
	for (const FFBXTriangle& Triangle : RawMesh.VertexData)
	{
		MaterialIndices.insert(Triangle.MaterialID);
		FFBXMesh& SplitMesh = MeshByMaterial[Triangle.MaterialID];
		SplitMesh.MaterialID = Triangle.MaterialID;
		SplitMesh.VertexData.push_back(Triangle);
	}

	for (auto& MeshPair : MeshByMaterial)
	{
		GetIndex(MeshPair.second);
		InModel.MeshData.push_back(std::move(MeshPair.second));
	}

	GetMaterial(InNode, MaterialIndices, InModel);
}


void RecursiveLoadMesh(FbxNode* InNode, FFBXRenderData& OutData)
{
	//XML
	if (InNode->GetNodeAttribute() == NULL)
	{
		//NULL Node
	}
	else
	{
		FbxNodeAttribute::EType AttributeType = (InNode->GetNodeAttribute()->GetAttributeType());

		if (AttributeType == fbxsdk::FbxNodeAttribute::eMesh)
		{
			OutData.ModelData.push_back(FFBXModel());
			FFBXModel& InModel = OutData.ModelData[OutData.ModelData.size() - 1];
			GetMesh(InNode, InModel);
		}
		else if (AttributeType == fbxsdk::FbxNodeAttribute::eSkeleton)
		{

		}
	}

	for (int i = 0; i < InNode->GetChildCount(); i++)
	{
		RecursiveLoadMesh(InNode->GetChild(i), OutData);
	}
}

void DestroySdkObjects(FbxManager* InManager)
{
	if (InManager)
	{
		InManager->Destroy();
	}
}

// 内部用：FBX SDKを使って実際にメッシュデータを解析する
// Internal: parse mesh data directly via FBX SDK (no cache)
static void ParseFBXFile(const char* InPath, FFBXRenderData& OutData)
{
	FbxManager* SdkManager = NULL;
	FbxScene* Scene = NULL;

	InitializeSdkObjects(SdkManager, Scene);

	FbxString FBXPath(InPath);
	LoadScene(SdkManager, Scene, FBXPath.Buffer());

	FbxGeometryConverter Converter(SdkManager);
	Converter.Triangulate(Scene, true);

	if (FbxNode* Node = Scene->GetRootNode())
	{
		for (int i = 0; i < Node->GetChildCount(); i++)
		{
			RecursiveLoadMesh(Node->GetChild(i), OutData);
		}
	}

	DestroySdkObjects(SdkManager);
}

// ファイルの最終更新時刻を取得する (失敗時は 0)
// Returns the last-modified time of a file, or 0 on failure
static int64_t GetFileModTime(const char* InPath)
{
	struct stat FileStat;
	if (::stat(InPath, &FileStat) != 0)
	{
		return 0;
	}
	return static_cast<int64_t>(FileStat.st_mtime);
}

// ペイロードのチェックサムを計算する (XOR折り畳み方式)
// Compute a lightweight XOR-fold checksum over raw bytes
static uint32_t ComputeChecksum(const std::vector<char>& InBytes)
{
	uint32_t Sum = 0;
	for (size_t i = 0; i < InBytes.size(); i += 4)
	{
		uint32_t Word = 0;
		size_t Remaining = std::min<size_t>(4, InBytes.size() - i);
		memcpy(&Word, InBytes.data() + i, Remaining);
		Sum ^= Word;
	}
	return Sum;
}

// minizを用いてバッファを圧縮する
// Compress a raw byte buffer using miniz
static bool CompressBuffer(const std::vector<char>& InBuffer, std::vector<char>& OutCompressed)
{
	if (InBuffer.empty())
	{
		OutCompressed.clear();
		return true;
	}

	mz_ulong BoundSize = compressBound(static_cast<mz_ulong>(InBuffer.size()));
	OutCompressed.resize(static_cast<size_t>(BoundSize));

	mz_ulong CompressedSize = BoundSize;
	int Result = compress2(
		reinterpret_cast<mz_uint8*>(OutCompressed.data()),
		&CompressedSize,
		reinterpret_cast<const mz_uint8*>(InBuffer.data()),
		static_cast<mz_ulong>(InBuffer.size()),
		MZ_BEST_COMPRESSION
	);

	if (Result != MZ_OK)
	{
		OutCompressed.clear();
		return false;
	}

	OutCompressed.resize(static_cast<size_t>(CompressedSize));
	return true;
}

// 圧縮バッファを解凍する
// Decompress a raw byte buffer using miniz
static bool DecompressBuffer(
	const std::vector<char>& InCompressed,
	uint32_t InUncompressedSize,
	std::vector<char>& OutBuffer)
{
	if (InUncompressedSize == 0)
	{
		OutBuffer.clear();
		return true;
	}

	OutBuffer.resize(InUncompressedSize);

	mz_ulong DestLen = static_cast<mz_ulong>(InUncompressedSize);
	int Result = uncompress(
		reinterpret_cast<mz_uint8*>(OutBuffer.data()),
		&DestLen,
		reinterpret_cast<const mz_uint8*>(InCompressed.data()),
		static_cast<mz_ulong>(InCompressed.size())
	);

	if (Result != MZ_OK || DestLen != InUncompressedSize)
	{
		OutBuffer.clear();
		return false;
	}

	return true;
}

// ---- プリミティブ書き込みヘルパー ----
// ---- Primitive write helpers ----

template<typename T>
static void WriteValue(std::ofstream& Stream, const T& Value)
{
	Stream.write(reinterpret_cast<const char*>(&Value), sizeof(T));
}

static void WriteString(std::ofstream& Stream, const std::string& Str)
{
	// 文字列長 (uint32_t) + 内容
	// Write length (uint32_t) then raw bytes
	uint32_t Len = static_cast<uint32_t>(Str.size());
	WriteValue(Stream, Len);
	if (Len > 0)
	{
		Stream.write(Str.data(), Len);
	}
}

template<typename T>
static void WriteVector(std::ofstream& Stream, const std::vector<T>& Vec)
{
	// 要素数 (uint32_t) + PODデータ一括書き込み
	// Write element count then the entire POD array at once
	uint32_t Count = static_cast<uint32_t>(Vec.size());
	WriteValue(Stream, Count);
	if (Count > 0)
	{
		Stream.write(reinterpret_cast<const char*>(Vec.data()), Count * sizeof(T));
	}
}

// ---- プリミティブ読み込みヘルパー ----
// ---- Primitive read helpers ----

template<typename T>
static bool ReadValue(std::ifstream& Stream, T& OutValue)
{
	Stream.read(reinterpret_cast<char*>(&OutValue), sizeof(T));
	return Stream.good();
}

static bool ReadString(std::ifstream& Stream, std::string& OutStr)
{
	uint32_t Len = 0;
	if (!ReadValue(Stream, Len)) { return false; }
	OutStr.resize(Len);
	if (Len > 0)
	{
		Stream.read(&OutStr[0], Len);
	}
	return Stream.good();
}

template<typename T>
static bool ReadVector(std::ifstream& Stream, std::vector<T>& OutVec)
{
	uint32_t Count = 0;
	if (!ReadValue(Stream, Count)) { return false; }
	OutVec.resize(Count);
	if (Count > 0)
	{
		Stream.read(reinterpret_cast<char*>(OutVec.data()), Count * sizeof(T));
	}
	return Stream.good();
}

// ---- 構造体ごとの書き込み ----
// ---- Per-struct write functions ----

static void WriteMaterial(std::ofstream& Stream, const FFBXMaterial& Mat)
{
	WriteString(Stream, Mat.DiffuseMapFileName);
	WriteString(Stream, Mat.SpecularMapFileName);
	WriteString(Stream, Mat.AlphaMapFileName);
	WriteString(Stream, Mat.BumpMapFileName);
}

static void WriteMesh(std::ofstream& Stream, const FFBXMesh& Mesh)
{
	WriteValue(Stream, Mesh.MaterialID);
	WriteVector(Stream, Mesh.VertexData);  // FFBXTriangle は純粋な POD
	WriteVector(Stream, Mesh.IndexData);
}

static void WriteModel(std::ofstream& Stream, const FFBXModel& Model)
{
	// メッシュ配列
	uint32_t MeshCount = static_cast<uint32_t>(Model.MeshData.size());
	WriteValue(Stream, MeshCount);
	for (const FFBXMesh& Mesh : Model.MeshData)
	{
		WriteMesh(Stream, Mesh);
	}

	// マテリアルマップ (キー int, 値 FFBXMaterial)
	uint32_t MatCount = static_cast<uint32_t>(Model.MaterialMap.size());
	WriteValue(Stream, MatCount);
	for (const auto& Pair : Model.MaterialMap)
	{
		WriteValue(Stream, Pair.first);
		WriteMaterial(Stream, Pair.second);
	}
}

static void WriteRenderData(std::ofstream& Stream, const FFBXRenderData& Data)
{
	uint32_t ModelCount = static_cast<uint32_t>(Data.ModelData.size());
	WriteValue(Stream, ModelCount);
	for (const FFBXModel& Model : Data.ModelData)
	{
		WriteModel(Stream, Model);
	}
}

// ---- 構造体ごとの読み込み ----
// ---- Per-struct read functions ----

static bool ReadMaterial(std::ifstream& Stream, FFBXMaterial& Mat)
{
	return ReadString(Stream, Mat.DiffuseMapFileName)
		&& ReadString(Stream, Mat.SpecularMapFileName)
		&& ReadString(Stream, Mat.AlphaMapFileName)
		&& ReadString(Stream, Mat.BumpMapFileName);
}

static bool ReadMesh(std::ifstream& Stream, FFBXMesh& Mesh)
{
	if (!ReadValue(Stream, Mesh.MaterialID)) { return false; }
	if (!ReadVector(Stream, Mesh.VertexData)) { return false; }
	if (!ReadVector(Stream, Mesh.IndexData)) { return false; }
	return true;
}

static bool ReadModel(std::ifstream& Stream, FFBXModel& Model)
{
	uint32_t MeshCount = 0;
	if (!ReadValue(Stream, MeshCount)) { return false; }
	Model.MeshData.resize(MeshCount);
	for (FFBXMesh& Mesh : Model.MeshData)
	{
		if (!ReadMesh(Stream, Mesh)) { return false; }
	}

	uint32_t MatCount = 0;
	if (!ReadValue(Stream, MatCount)) { return false; }
	for (uint32_t i = 0; i < MatCount; ++i)
	{
		int Key = 0;
		if (!ReadValue(Stream, Key)) { return false; }
		FFBXMaterial Mat;
		if (!ReadMaterial(Stream, Mat)) { return false; }
		Model.MaterialMap[Key] = std::move(Mat);
	}
	return true;
}

static bool ReadRenderData(std::ifstream& Stream, FFBXRenderData& Data)
{
	uint32_t ModelCount = 0;
	if (!ReadValue(Stream, ModelCount)) { return false; }
	Data.ModelData.resize(ModelCount);
	for (FFBXModel& Model : Data.ModelData)
	{
		if (!ReadModel(Stream, Model)) { return false; }
	}
	return true;
}


// ---------------------------------------------------------------------------
//  FFBXAssetImport 実装
// ---------------------------------------------------------------------------

// FBXパスからキャッシュパスを自動生成する（拡張子を .fbxcache に置換）
// Derive cache path from FBX path by replacing the extension with .fbxcache
static std::string DeriveCachePath(const char* InFBXPath)
{
	std::string CachePath(InFBXPath);
	size_t DotPos = CachePath.rfind('.');
	if (DotPos != std::string::npos)
	{
		CachePath = CachePath.substr(0, DotPos);
	}
	CachePath += ".fbxcache";
	return CachePath;
}






void FFBXAssetImport::LoadMeshData(const char* InPath, FFBXRenderData& OutData)
{
	// キャッシュパスを自動生成（例: model.fbx → model.fbxcache）
	// Auto-derive cache path (e.g. model.fbx → model.fbxcache)
	std::string CachePath = DeriveCachePath(InPath);

	// キャッシュが存在・有効であればそこから読み込む（高速パス）
	// Fast path: load from binary cache if it exists and is up-to-date
	if (LoadFromCache(CachePath.c_str(), InPath, OutData))
	{
		return;
	}

	// キャッシュが無い／古い／破損している場合は FBX を解析する（低速パス）
	// Slow path: parse FBX and save result as binary cache for next time
	ParseFBXFile(InPath, OutData);
	SaveToCache(CachePath.c_str(), InPath, OutData);
}


bool FFBXAssetImport::SaveToCache(const char* InCachePath, const char* InOriginalFBXPath, const FFBXRenderData& InData)
{
	std::vector<char> PayloadBuffer;
	{
		struct VecStream
		{
			std::vector<char>& Buf;

			void write(const char* Data, size_t Len)
			{
				Buf.insert(Buf.end(), Data, Data + Len);
			}

			bool good() const { return true; }
		} VS{ PayloadBuffer };

		auto WriteValueV = [&VS](const auto& V) {
			VS.write(reinterpret_cast<const char*>(&V), sizeof(V));
			};

		auto WriteStringV = [&VS, &WriteValueV](const std::string& Str) {
			uint32_t Len = static_cast<uint32_t>(Str.size());
			WriteValueV(Len);
			if (Len > 0) { VS.write(Str.data(), Len); }
			};

		std::function<void(const FFBXMaterial&)> WriteMatV = [&](const FFBXMaterial& Mat) {
			WriteStringV(Mat.DiffuseMapFileName);
			WriteStringV(Mat.SpecularMapFileName);
			WriteStringV(Mat.AlphaMapFileName);
			WriteStringV(Mat.BumpMapFileName);
			};

		std::function<void(const FFBXMesh&)> WriteMeshV = [&](const FFBXMesh& Mesh) {
			WriteValueV(Mesh.MaterialID);

			uint32_t TC = static_cast<uint32_t>(Mesh.VertexData.size());
			WriteValueV(TC);
			if (TC > 0)
			{
				VS.write(reinterpret_cast<const char*>(Mesh.VertexData.data()), TC * sizeof(FFBXTriangle));
			}

			uint32_t IC = static_cast<uint32_t>(Mesh.IndexData.size());
			WriteValueV(IC);
			if (IC > 0)
			{
				VS.write(reinterpret_cast<const char*>(Mesh.IndexData.data()), IC * sizeof(uint16_t));
			}
			};

		std::function<void(const FFBXModel&)> WriteModelV = [&](const FFBXModel& Model) {
			uint32_t MC = static_cast<uint32_t>(Model.MeshData.size());
			WriteValueV(MC);
			for (const FFBXMesh& M : Model.MeshData)
			{
				WriteMeshV(M);
			}

			uint32_t MatC = static_cast<uint32_t>(Model.MaterialMap.size());
			WriteValueV(MatC);
			for (const auto& P : Model.MaterialMap)
			{
				WriteValueV(P.first);
				WriteMatV(P.second);
			}
			};

		uint32_t ModelCount = static_cast<uint32_t>(InData.ModelData.size());
		WriteValueV(ModelCount);
		for (const FFBXModel& Mdl : InData.ModelData)
		{
			WriteModelV(Mdl);
		}
	}

	// 压缩
	std::vector<char> FinalPayload;
	bool bUseCompressed = false;

	std::vector<char> CompressedPayload;
	if (CompressBuffer(PayloadBuffer, CompressedPayload))
	{
		if (CompressedPayload.size() < PayloadBuffer.size())
		{
			FinalPayload = std::move(CompressedPayload);
			bUseCompressed = true;
		}
		else
		{
			FinalPayload = PayloadBuffer;
		}
	}
	else
	{
		FinalPayload = PayloadBuffer;
	}

	FFBXCacheHeader Header;
	Header.Magic = FFBXCacheHeader::MAGIC_VALUE;
	Header.Version = FFBXCacheHeader::CACHE_VERSION;
	Header.SourceFileTime = GetFileModTime(InOriginalFBXPath);
	Header.UncompressedSize = static_cast<uint32_t>(PayloadBuffer.size());
	Header.CompressedSize = static_cast<uint32_t>(FinalPayload.size());
	Header.bCompressed = bUseCompressed ? 1u : 0u;
	Header.Checksum = ComputeChecksum(FinalPayload);

	std::ofstream OutFile(InCachePath, std::ios::binary | std::ios::trunc);
	if (!OutFile.is_open())
	{
		return false;
	}

	OutFile.write(reinterpret_cast<const char*>(&Header), sizeof(FFBXCacheHeader));

	if (!FinalPayload.empty())
	{
		OutFile.write(FinalPayload.data(), static_cast<std::streamsize>(FinalPayload.size()));
	}

	return OutFile.good();
}

bool FFBXAssetImport::LoadFromCache(const char* InCachePath, const char* InOriginalFBXPath, FFBXRenderData& OutData)
{
	std::ifstream InFile(InCachePath, std::ios::binary);
	if (!InFile.is_open())
	{
		return false;
	}

	FFBXCacheHeader Header;
	InFile.read(reinterpret_cast<char*>(&Header), sizeof(FFBXCacheHeader));
	if (!InFile.good())
	{
		return false;
	}

	if (Header.Magic != FFBXCacheHeader::MAGIC_VALUE)
	{
		return false;
	}

	if (Header.Version != FFBXCacheHeader::CACHE_VERSION)
	{
		return false;
	}

	int64_t CurrentFileTime = GetFileModTime(InOriginalFBXPath);
	if (CurrentFileTime != 0 && Header.SourceFileTime != CurrentFileTime)
	{
		return false;
	}

	std::vector<char> StoredPayload(
		(std::istreambuf_iterator<char>(InFile)),
		std::istreambuf_iterator<char>()
	);

	if (StoredPayload.size() != Header.CompressedSize)
	{
		return false;
	}

	if (ComputeChecksum(StoredPayload) != Header.Checksum)
	{
		return false;
	}

	std::vector<char> PayloadBuffer;
	if (Header.bCompressed)
	{
		if (!DecompressBuffer(StoredPayload, Header.UncompressedSize, PayloadBuffer))
		{
			return false;
		}
	}
	else
	{
		PayloadBuffer = std::move(StoredPayload);

		if (PayloadBuffer.size() != Header.UncompressedSize)
		{
			return false;
		}
	}

	struct ReadStream
	{
		const char* Ptr;
		size_t Remaining;
		bool bOk = true;

		void read(char* Dst, size_t Len)
		{
			if (Len > Remaining)
			{
				bOk = false;
				return;
			}

			memcpy(Dst, Ptr, Len);
			Ptr += Len;
			Remaining -= Len;
		}

		bool good() const { return bOk; }
	} RS{ PayloadBuffer.data(), PayloadBuffer.size(), true };

	auto ReadValueRS = [&RS](auto& V) -> bool {
		RS.read(reinterpret_cast<char*>(&V), sizeof(V));
		return RS.good();
		};

	auto ReadStringRS = [&RS, &ReadValueRS](std::string& Str) -> bool {
		uint32_t Len = 0;
		if (!ReadValueRS(Len)) { return false; }

		Str.resize(Len);
		if (Len > 0)
		{
			RS.read(&Str[0], Len);
		}
		return RS.good();
		};

	std::function<bool(FFBXMaterial&)> ReadMatRS = [&](FFBXMaterial& Mat) -> bool {
		return ReadStringRS(Mat.DiffuseMapFileName)
			&& ReadStringRS(Mat.SpecularMapFileName)
			&& ReadStringRS(Mat.AlphaMapFileName)
			&& ReadStringRS(Mat.BumpMapFileName);
		};

	std::function<bool(FFBXMesh&)> ReadMeshRS = [&](FFBXMesh& Mesh) -> bool {
		if (!ReadValueRS(Mesh.MaterialID)) { return false; }

		uint32_t TC = 0;
		if (!ReadValueRS(TC)) { return false; }
		Mesh.VertexData.resize(TC);
		if (TC > 0)
		{
			RS.read(reinterpret_cast<char*>(Mesh.VertexData.data()), TC * sizeof(FFBXTriangle));
		}

		uint32_t IC = 0;
		if (!ReadValueRS(IC)) { return false; }
		Mesh.IndexData.resize(IC);
		if (IC > 0)
		{
			RS.read(reinterpret_cast<char*>(Mesh.IndexData.data()), IC * sizeof(uint16_t));
		}

		return RS.good();
		};

	std::function<bool(FFBXModel&)> ReadModelRS = [&](FFBXModel& Model) -> bool {
		uint32_t MC = 0;
		if (!ReadValueRS(MC)) { return false; }

		Model.MeshData.resize(MC);
		for (FFBXMesh& M : Model.MeshData)
		{
			if (!ReadMeshRS(M)) { return false; }
		}

		uint32_t MatC = 0;
		if (!ReadValueRS(MatC)) { return false; }

		for (uint32_t i = 0; i < MatC; ++i)
		{
			int Key = 0;
			if (!ReadValueRS(Key)) { return false; }

			FFBXMaterial Mat;
			if (!ReadMatRS(Mat)) { return false; }

			Model.MaterialMap[Key] = std::move(Mat);
		}

		return true;
		};

	uint32_t ModelCount = 0;
	if (!ReadValueRS(ModelCount)) { return false; }

	OutData.ModelData.resize(ModelCount);
	for (FFBXModel& Model : OutData.ModelData)
	{
		if (!ReadModelRS(Model)) { return false; }
	}

	return true;
}

void FFBXAssetImport::LoadMeshDataCached(const char* InFBXPath, const char* InCachePath, FFBXRenderData& OutData)
{
	// キャッシュが有効であればそちらから即座に読み込む
	// Fast path: load from binary cache when it is up-to-date and valid
	if (LoadFromCache(InCachePath, InFBXPath, OutData))
	{
		return;
	}

	// キャッシュミス時: FBX を解析して指定パスに保存
	// Cache miss: parse FBX and save to the explicitly specified cache path
	ParseFBXFile(InFBXPath, OutData);
	SaveToCache(InCachePath, InFBXPath, OutData);
}


// ---------------------------------------------------------------------------
//  バイナリシリアライズ ユーティリティ
//  Binary serialization utilities
// ---------------------------------------------------------------------------
