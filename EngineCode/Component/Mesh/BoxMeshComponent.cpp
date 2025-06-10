#include "BoxMeshComponent.h"
#include "../../Mesh/Core/MeshType.h"

CBoxMeshComponent::CBoxMeshComponent()
{

}

void CBoxMeshComponent::CreateMesh(FMeshRenderingData& MeshData, float InHeight, float InWidth, float InDepth)
{
	float CHeight = 0.5f * InHeight;
	float CWidth = 0.5f * InWidth;
	float CDepth = 0.5f * InDepth;

	//构建我们的顶点
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, -CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, +CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, +CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, -CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f)));

	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, -CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, -CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, +CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, +CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)));

	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, +CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, +CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, +CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, +CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)));

	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, -CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, -CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, -CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, -CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)));

	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, -CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, +CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, +CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(-CWidth, -CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)));

	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, -CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, +CHeight, -CDepth), XMFLOAT4(Colors::White), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, +CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)));
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(+CWidth, -CHeight, +CDepth), XMFLOAT4(Colors::White), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)));

	//构建我们的索引
	//前
	MeshData.IndexData.push_back(0); MeshData.IndexData.push_back(1); MeshData.IndexData.push_back(2);
	MeshData.IndexData.push_back(0); MeshData.IndexData.push_back(2); MeshData.IndexData.push_back(3);

	//后
	MeshData.IndexData.push_back(4); MeshData.IndexData.push_back(5); MeshData.IndexData.push_back(6);
	MeshData.IndexData.push_back(4); MeshData.IndexData.push_back(6); MeshData.IndexData.push_back(7);

	//左
	MeshData.IndexData.push_back(8); MeshData.IndexData.push_back(9); MeshData.IndexData.push_back(10);
	MeshData.IndexData.push_back(8); MeshData.IndexData.push_back(10); MeshData.IndexData.push_back(11);

	//右
	MeshData.IndexData.push_back(12); MeshData.IndexData.push_back(13); MeshData.IndexData.push_back(14);
	MeshData.IndexData.push_back(12); MeshData.IndexData.push_back(14); MeshData.IndexData.push_back(15);

	//上
	MeshData.IndexData.push_back(16); MeshData.IndexData.push_back(17); MeshData.IndexData.push_back(18);
	MeshData.IndexData.push_back(16); MeshData.IndexData.push_back(18); MeshData.IndexData.push_back(19);

	//下
	MeshData.IndexData.push_back(20); MeshData.IndexData.push_back(21); MeshData.IndexData.push_back(22);
	MeshData.IndexData.push_back(20); MeshData.IndexData.push_back(22); MeshData.IndexData.push_back(23);
}


void CBoxMeshComponent::BuildKey(size_t& OutHashKey, float InHeight, float InWidth, float InDepth)
{
	std::hash<float> FloatHash;

	OutHashKey = 1;
	OutHashKey += FloatHash(InHeight);
	OutHashKey += FloatHash(InWidth);
	OutHashKey += FloatHash(InDepth);
}
