#pragma once

#include <vector>
#include <string>
#include <map>

#ifdef FBXSDK_EXPORTS
#define FBXASSETIMPORT_API __declspec(dllexport)
#else
#define FBXASSETIMPORT_API __declspec(dllimport)
#endif 
// 2D向量
struct FFBXVector2
{
	FFBXVector2()
		:X(0.f)
		, Y(0.f)
	{}
	float X;
	float Y;
};

//3D向量
struct FFBXVector3 :public FFBXVector2
{
	FFBXVector3()
		:FFBXVector2()
		, Z(0.f)
	{}

	float Z;
};

// 顶点
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
struct FBXASSETIMPORT_API FFBXTriangle
{
	FFBXTriangle()
		:MaterialID(0)
	{}

	FFBXVertex Vertexs[3];
	int MaterialID;
};

// 网格数据
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
struct FBXASSETIMPORT_API FFBXMaterial
{
	std::string DiffuseMapFileName;
	std::string SpecularMapFileName;
	std::string AlphaMapFileName;
	std::string BumpMapFileName;
};

// 带材质的模型数据
struct FBXASSETIMPORT_API FFBXModel
{
	std::vector<FFBXMesh> MeshData;
	std::map<int, FFBXMaterial> MaterialMap;
};

// FBX渲染数据
struct FBXASSETIMPORT_API FFBXRenderData
{
	std::vector<FFBXModel> ModelData;
};

// SDK 版本
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
	FBXASSETIMPORT_API void LoadMeshData(const char* InPath, FFBXRenderData& OutData);
};
