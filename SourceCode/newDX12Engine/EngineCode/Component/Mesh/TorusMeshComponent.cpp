#include "TorusMeshComponent.h"
#include "../../Mesh/Core/MeshType.h"


CTorusMeshComponent::CTorusMeshComponent()
{

}

void CTorusMeshComponent::CreateMesh(FMeshRenderingData& MeshData, float InRadius, float InSectionRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
	float BetaValue = XM_2PI / (float)InAxialSubdivision;
	float ThetaValue = XM_2PI / (float)InHeightSubdivision;
	for (size_t i = 0; i <= InAxialSubdivision; ++i)
	{
		float BetaRadian = i * BetaValue;

		//InRadius
		fvector_3d Center(
			InRadius * cosf(BetaRadian),
			0.f,
			InRadius * sinf(BetaRadian));

		for (size_t j = 0; j <= InHeightSubdivision; ++j)
		{
			float ThetaRadian = j * ThetaValue;

			float ThetaBetaValueCosCos = cosf(ThetaRadian) * cosf(BetaRadian);
			float ThetaBetaValueCosSin = cosf(ThetaRadian) * sinf(BetaRadian);
			float ThetaValueSin = sinf(ThetaRadian);

			// 拿到点的位置
			// 頂点の位置を取得
			fvector_3d PointPosition(
				Center.x + InSectionRadius * ThetaBetaValueCosCos,
				Center.y + InSectionRadius * ThetaValueSin,
				Center.z + InSectionRadius * ThetaBetaValueCosSin);

			MeshData.VertexData.push_back(
				FVertex(EngineMath::ToFloat3(PointPosition),
					XMFLOAT4(Colors::White)));

			FVertex& InVertex = MeshData.VertexData[MeshData.VertexData.size() - 1];


			// 求法线
			// 法線を計算
			fvector_3d Normal = PointPosition - Center;
			Normal.normalize();

			// 法线赋值
			// 法線を設定
			InVertex.Normal = EngineMath::ToFloat3(Normal);

			// 展UV
			// UV展開
			InVertex.TexCoord.x = (float)j / (float)InHeightSubdivision;
			InVertex.TexCoord.y = (float)i / (float)InAxialSubdivision;

			//InVertex.UTangent.x = tan(BetaRadian) * InRadius;
			//InVertex.UTangent.y = tan(ThetaRadian) * InSectionRadius;
		}
	}

	for (size_t i = 0; i <= InAxialSubdivision; ++i)
	{
		for (size_t j = 0; j < InHeightSubdivision; ++j)
		{
			// 绘制外圈两个三角形
			// 外周の二つの三角形を描画
			DrawQuadrilateral(
				MeshData,//提取绘制信息  // 描画情報を取得
				GetQuadrilateralDrawPointTypeA(j, i, InHeightSubdivision));//拿到四个点  // 4つの頂点を取得
		}
	}
}

void CTorusMeshComponent::BuildKey(size_t& OutHashKey, float InRadius, float InSectionRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
	std::hash<float> FloatHash;
	std::hash<int> IntHash;

	OutHashKey = 9;
	OutHashKey += FloatHash(InRadius);
	OutHashKey += FloatHash(InSectionRadius);

	OutHashKey += IntHash._Do_hash(InAxialSubdivision);
	OutHashKey += IntHash._Do_hash(InHeightSubdivision);
}
