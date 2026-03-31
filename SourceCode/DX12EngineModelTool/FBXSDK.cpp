#include "ThirdPartyLibrary/FBX/include/fbxsdk.h"
#include "FBXSDK.h"

#pragma comment(lib, "libfbxsdk.lib")
#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libfbxsdk-mt.lib")

#include <sys/stat.h>   
#include <cstring>     
#include <numeric>      

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

	int VertexID = 0;
	for (int i = 0; i < PolygonCount; i++)//Get Polygon
	{
		OutData.VertexData.push_back(FFBXTriangle());
		FFBXTriangle& InTriangle = OutData.VertexData[OutData.VertexData.size() - 1];

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
					int TextureUVIndex = InMesh->GetTextureUVIndex(i, j);
					FbxVector2 UV = TextureUV->GetDirectArray().GetAt(TextureUVIndex);
					switch (ReferenceMode)
					{
					case fbxsdk::FbxLayerElement::eDirect:
					case fbxsdk::FbxLayerElement::eIndexToDirect:
					{
						FbxVector2 UV = TextureUV->GetDirectArray().GetAt(ControlPointIndex);

						InTriangle.Vertexs[j].UV.X = UV.mData[0];
						InTriangle.Vertexs[j].UV.Y = 1.f - UV.mData[1];//UV取反 //UVを反転
						break;
					}
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

void GetMaterial() {}

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

	InModel.MeshData.push_back(FFBXMesh());
	FFBXMesh& InMesh = InModel.MeshData[InModel.MeshData.size() - 1];

	GetPolygons(NodeMesh, InMesh);

	GetIndex(InMesh);

	//GetMaterial();
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

	// 簡易実装: 直接ファイルに書き出してチェックサムは省略可能だが、
	// ここではペイロードを std::vector<char> に溜めて後でチェックサムを計算する
	// Simple approach: collect payload bytes, compute checksum, then write header + payload
	std::vector<char> PayloadBuffer;
	{
		// vectorへの書き込み用ヘルパー (小さなラムダストリーム)
		struct VecStream
		{
			std::vector<char>& Buf;

			void write(const char* Data, size_t Len)
			{
				Buf.insert(Buf.end(), Data, Data + Len);
			}

			bool good() const { return true; }
		} VS{ PayloadBuffer };

		// ペイロードを VecStream に書き出す
		// Serialize render data into the vector
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
			if (TC > 0) { VS.write(reinterpret_cast<const char*>(Mesh.VertexData.data()), TC * sizeof(FFBXTriangle)); }
			uint32_t IC = static_cast<uint32_t>(Mesh.IndexData.size());
			WriteValueV(IC);
			if (IC > 0) { VS.write(reinterpret_cast<const char*>(Mesh.IndexData.data()), IC * sizeof(uint16_t)); }
			};

		std::function<void(const FFBXModel&)> WriteModelV = [&](const FFBXModel& Model) {
			uint32_t MC = static_cast<uint32_t>(Model.MeshData.size());
			WriteValueV(MC);
			for (const FFBXMesh& M : Model.MeshData) { WriteMeshV(M); }
			uint32_t MatC = static_cast<uint32_t>(Model.MaterialMap.size());
			WriteValueV(MatC);
			for (const auto& P : Model.MaterialMap) { WriteValueV(P.first); WriteMatV(P.second); }
			};

		uint32_t ModelCount = static_cast<uint32_t>(InData.ModelData.size());
		WriteValueV(ModelCount);
		for (const FFBXModel& Mdl : InData.ModelData) { WriteModelV(Mdl); }
	}

	// ---- ヘッダーを組み立てる ----
	// ---- Build the cache header ----
	FFBXCacheHeader Header;
	Header.Magic = FFBXCacheHeader::MAGIC_VALUE;
	Header.Version = FFBXCacheHeader::CACHE_VERSION;
	Header.SourceFileTime = GetFileModTime(InOriginalFBXPath);
	Header.Checksum = ComputeChecksum(PayloadBuffer);

	// ---- ファイルに書き出す ----
	// ---- Write header + payload to disk ----
	std::ofstream OutFile(InCachePath, std::ios::binary | std::ios::trunc);
	if (!OutFile.is_open())
	{
		return false;
	}

	OutFile.write(reinterpret_cast<const char*>(&Header), sizeof(FFBXCacheHeader));
	OutFile.write(PayloadBuffer.data(), static_cast<std::streamsize>(PayloadBuffer.size()));

	return OutFile.good();
}

bool FFBXAssetImport::LoadFromCache(const char* InCachePath, const char* InOriginalFBXPath, FFBXRenderData& OutData)
{
	std::ifstream InFile(InCachePath, std::ios::binary);
	if (!InFile.is_open())
	{
		// キャッシュファイルが存在しない
		return false;
	}

	// ---- ヘッダーを検証する ----
	// ---- Validate the header ----
	FFBXCacheHeader Header;
	InFile.read(reinterpret_cast<char*>(&Header), sizeof(FFBXCacheHeader));
	if (!InFile.good())
	{
		return false;
	}

	// マジックナンバーチェック
	if (Header.Magic != FFBXCacheHeader::MAGIC_VALUE)
	{
		return false;
	}

	// バージョンチェック
	if (Header.Version != FFBXCacheHeader::CACHE_VERSION)
	{
		return false;
	}

	// タイムスタンプチェック: 元のFBXが更新されていたらキャッシュを無効化
	// Invalidate cache if the source FBX has been modified
	int64_t CurrentFileTime = GetFileModTime(InOriginalFBXPath);
	if (CurrentFileTime != 0 && Header.SourceFileTime != CurrentFileTime)
	{
		return false;
	}

	// ---- ペイロードをメモリに読み込む (チェックサム検証のため) ----
	// ---- Read entire payload for checksum verification ----
	std::vector<char> PayloadBuffer(
		(std::istreambuf_iterator<char>(InFile)),
		std::istreambuf_iterator<char>()
	);

	if (ComputeChecksum(PayloadBuffer) != Header.Checksum)
	{
		// データ破損
		return false;
	}

	// ---- ペイロードをデシリアライズする ----
	// ---- Deserialize from the buffer ----
	struct ReadStream
	{
		const char* Ptr;
		size_t      Remaining;

		void read(char* Dst, size_t Len)
		{
			if (Len > Remaining) { Len = Remaining; }
			memcpy(Dst, Ptr, Len);
			Ptr += Len;
			Remaining -= Len;
		}

		bool good() const { return Remaining < static_cast<size_t>(-1); }
	} RS{ PayloadBuffer.data(), PayloadBuffer.size() };

	auto ReadValueRS = [&RS](auto& V) -> bool {
		RS.read(reinterpret_cast<char*>(&V), sizeof(V));
		return RS.good();
		};

	auto ReadStringRS = [&RS, &ReadValueRS](std::string& Str) -> bool {
		uint32_t Len = 0;
		if (!ReadValueRS(Len)) { return false; }
		Str.resize(Len);
		if (Len > 0) { RS.read(&Str[0], Len); }
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
		uint32_t TC = 0; if (!ReadValueRS(TC)) { return false; }
		Mesh.VertexData.resize(TC);
		if (TC > 0) { RS.read(reinterpret_cast<char*>(Mesh.VertexData.data()), TC * sizeof(FFBXTriangle)); }
		uint32_t IC = 0; if (!ReadValueRS(IC)) { return false; }
		Mesh.IndexData.resize(IC);
		if (IC > 0) { RS.read(reinterpret_cast<char*>(Mesh.IndexData.data()), IC * sizeof(uint16_t)); }
		return RS.good();
		};

	std::function<bool(FFBXModel&)> ReadModelRS = [&](FFBXModel& Model) -> bool {
		uint32_t MC = 0; if (!ReadValueRS(MC)) { return false; }
		Model.MeshData.resize(MC);
		for (FFBXMesh& M : Model.MeshData) { if (!ReadMeshRS(M)) { return false; } }
		uint32_t MatC = 0; if (!ReadValueRS(MatC)) { return false; }
		for (uint32_t i = 0; i < MatC; ++i) {
			int Key = 0; if (!ReadValueRS(Key)) { return false; }
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
