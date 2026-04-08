#pragma once

#include <vector>
#include <string>
#include <map>

#include <fstream>
#include <cstdint>
#include <functional>

#ifdef FBXSDK_EXPORTS
#define FBXASSETIMPORT_API __declspec(dllexport)
#else
#define FBXASSETIMPORT_API __declspec(dllimport)
#endif 
// 2D向量
// 2Dベクトル
struct FFBXVector2
{
	FFBXVector2()
		:X(0.f)
		, Y(0.f)
	{}
	float X;
	float Y;
};

// 3D向量
// 3Dベクトル
struct FFBXVector3 :public FFBXVector2
{
	FFBXVector3()
		:FFBXVector2()
		, Z(0.f)
	{}

	float Z;
};

// 頂点
struct FBXASSETIMPORT_API FFBXVertex
{
	FFBXVector3 Position;
	FFBXVector3 Color;
	FFBXVector3 Normal;
	FFBXVector3 Tangent;
	FFBXVector2 UV;
	FFBXVector3 Binormal;
};

// 三角图元信息
// 三角形プリミティブ情報
struct FBXASSETIMPORT_API FFBXTriangle
{
	FFBXTriangle()
		:MaterialID(0)
	{}

	FFBXVertex Vertexs[3];
	int MaterialID;
};

// 网格数据
// メッシュデータ
struct FBXASSETIMPORT_API FFBXMesh
{
	FFBXMesh()
		:MaterialID(0)
	{}
	std::vector<FFBXTriangle> VertexData;
	std::vector<uint16_t> IndexData;
	int MaterialID;
};

// 材质数据
// マテリアルデータ
struct FBXASSETIMPORT_API FFBXMaterial
{
	std::string DiffuseMapFileName;
	std::string SpecularMapFileName;
	std::string AlphaMapFileName;
	std::string BumpMapFileName;
};

// 带材质的模型数据
// マテリアル付きモデルデータ
struct FBXASSETIMPORT_API FFBXModel
{
	std::vector<FFBXMesh> MeshData;
	std::map<int, FFBXMaterial> MaterialMap;
};

// FBX渲染数据
// FBXレンダリングデータ
struct FBXASSETIMPORT_API FFBXRenderData
{
	std::vector<FFBXModel> ModelData;
};

// SDK 版本
// SDKバージョン
struct FBXASSETIMPORT_API FFBXVersion
{
	int Major;
	int Minor;
	int Revision;
};

//UE4 2.4
//7.5.0
struct FFBXAssetImport
{
	// FBXファイルからメッシュデータを読み込む
	// Load mesh data from an FBX file
	FBXASSETIMPORT_API void LoadMeshData(const char* InPath, FFBXRenderData& OutData);

	// バイナリキャッシュからデータを読み込む
	// Load mesh data from a binary cache file
	// Returns false if the cache is missing, outdated, or corrupt
	FBXASSETIMPORT_API bool LoadFromCache(const char* InCachePath, const char* InOriginalFBXPath, FFBXRenderData& OutData);

	// メッシュデータをバイナリキャッシュに書き出す
	// Save mesh data to a binary cache file
	// InOriginalFBXPath is used to record the source file's timestamp
	FBXASSETIMPORT_API bool SaveToCache(const char* InCachePath, const char* InOriginalFBXPath, const FFBXRenderData& InData);

	// FBXを読み込み、キャッシュがあればそちらを優先する高レベルAPI
	// High-level helper: loads from cache when valid, otherwise parses FBX and saves cache
	FBXASSETIMPORT_API void LoadMeshDataCached(const char* InFBXPath, const char* InCachePath, FFBXRenderData& OutData);
};

// バイナリキャッシュのヘッダー情報
// Binary cache header information
// 模型数据缓存
struct FBXASSETIMPORT_API FFBXCacheHeader
{
	// マジックナンバー: "FBXC" = 0x43584246
	// Magic number: "FBXC" = 0x43584246
	uint32_t Magic;

	// キャッシュフォーマットバージョン
	// Cache format version (increment when layout changes)
	uint32_t Version;

	// 元のFBXファイルの最終更新時刻 (time_t)
	// Last modified time of the original FBX file (time_t)
	int64_t SourceFileTime;

	// データチェックサム (単純な合計)
	// Simple checksum of data payload
	uint32_t Checksum;

	// 实际写入文件的 payload 大小
	// ファイルに書き込まれる実際のペイロードサイズ
	uint32_t CompressedSize;

	// 圧縮前の元のペイロードサイズ
	uint32_t UncompressedSize;
	// 0 = 未压缩, 1 = 已压缩
	// 0 = 非圧縮、1 = 圧縮
	uint32_t bCompressed;

	static constexpr uint32_t MAGIC_VALUE = 0x43584246u; // "FBXC"
	static constexpr uint32_t CACHE_VERSION = 8u;
};
