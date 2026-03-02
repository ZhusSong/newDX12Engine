#include "CustomMeshComponent.h"
#include "../../Mesh/Core/MeshType.h"

#if THIRD_PARTY_LIBRARY
#include "FBXSDK.h"

#pragma comment(lib, "DX12EngineModelTool.lib")

#endif // THIRD_PARTY_LIBRARY


CCustomMeshComponent::CCustomMeshComponent()
{

}

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

bool CCustomMeshComponent::LoadFBXFromBuff(const string& InPath, FMeshRenderingData& MeshData)
{
#if THIRD_PARTY_LIBRARY
	FFBXRenderData RenderData;
	FFBXAssetImport().LoadMeshData(InPath.c_str(), RenderData);

	for (auto& TmpModel : RenderData.ModelData)
	{
		for (auto& MeshTmp : TmpModel.MeshData)
		{
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

			// 索引
			// インデックス
			MeshData.IndexData = MeshTmp.IndexData;
		}
	}
#endif
	return true;
}