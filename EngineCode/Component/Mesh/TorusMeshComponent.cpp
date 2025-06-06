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

			fvector_3d PointPosition(
				Center.x + InSectionRadius * ThetaBetaValueCosCos,
				Center.y + InSectionRadius * ThetaValueSin,
				Center.z + InSectionRadius * ThetaBetaValueCosSin);

			MeshData.VertexData.push_back(
				FVertex(EngineMath::ToFloat3(PointPosition),
					XMFLOAT4(Colors::White)));

			FVertex& InVertex = MeshData.VertexData[MeshData.VertexData.size() - 1];

			fvector_3d Normal = PointPosition - Center;
			Normal.normalize();

			InVertex.Normal = EngineMath::ToFloat3(Normal);
		}
	}

	for (size_t i = 0; i <= InAxialSubdivision; ++i)
	{
		for (size_t j = 0; j < InHeightSubdivision; ++j)
		{
			DrawQuadrilateral(
				MeshData,
				GetQuadrilateralDrawPointTypeA(j, i, InHeightSubdivision));
		}
	}
}
