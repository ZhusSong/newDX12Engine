#include "PlaneMeshComponent.h"
#include "../../Mesh/Core/MeshType.h"

namespace
{
	XMMATRIX BuildPlaneRotationMatrix(const fvector_3d& InGenerateRotation)
	{
		return
			XMMatrixRotationX(XMConvertToRadians(InGenerateRotation.x)) *
			XMMatrixRotationY(XMConvertToRadians(InGenerateRotation.y)) *
			XMMatrixRotationZ(XMConvertToRadians(InGenerateRotation.z));
	}

	XMFLOAT3 TransformPlanePosition(const XMFLOAT3& InPosition, const XMMATRIX& InRotationMatrix)
	{
		XMFLOAT3 OutPosition;
		XMStoreFloat3(
			&OutPosition,
			XMVector3TransformCoord(XMLoadFloat3(&InPosition), InRotationMatrix));
		return OutPosition;
	}

	XMFLOAT3 TransformPlaneDirection(const XMFLOAT3& InDirection, const XMMATRIX& InRotationMatrix)
	{
		XMFLOAT3 OutDirection;
		XMStoreFloat3(
			&OutDirection,
			XMVector3TransformNormal(XMLoadFloat3(&InDirection), InRotationMatrix));
		return OutDirection;
	}
}

CPlaneMeshComponent::CPlaneMeshComponent()
{
}

void CPlaneMeshComponent::CreateMesh(
	FMeshRenderingData& MeshData,
	float InHeight,
	float InWidth,
	uint32_t InHeightSubdivide,
	uint32_t InWidthSubdivide,
	const fvector_3d& InGenerateRotation)
{
	auto SubdivideValue = [&](float InValue, uint32_t InSubdivideValue)->float
		{
			if (InSubdivideValue <= 1)
			{
				return InValue;
			}

			return InValue / ((float)InSubdivideValue - 1);
		};

	float CHeight = 0.5f * InHeight;
	float CWidth = 0.5f * InWidth;

	float HeightSubdivideValue = SubdivideValue(InHeight, InHeightSubdivide);
	float WidthSubdivideValue = SubdivideValue(InWidth, InWidthSubdivide);

	float HorizontalAverageSubdivision = 1.f / ((float)InWidthSubdivide - 1.f);
	float VerticalAverageSubdivision = 1.f / ((float)InHeightSubdivide - 1.f);
	XMMATRIX RotationMatrix = BuildPlaneRotationMatrix(InGenerateRotation);
	XMFLOAT3 PlaneNormal = TransformPlaneDirection(XMFLOAT3(0.f, 1.f, 0.f), RotationMatrix);
	XMFLOAT3 PlaneTangent = TransformPlaneDirection(XMFLOAT3(1.f, 0.f, 0.f), RotationMatrix);

	// 绘制点的位置
	// 頂点の位置を描画
	for (uint32_t i = 0; i < InHeightSubdivide; ++i)
	{
		float Z = CHeight - i * HeightSubdivideValue;
		for (uint32_t j = 0; j < InWidthSubdivide; ++j)
		{
			float X = CWidth - j * WidthSubdivideValue;
			XMFLOAT3 Position = TransformPlanePosition(
				XMFLOAT3(
					X,//x
					0.f,//y
					Z), //z
				RotationMatrix);
			MeshData.VertexData.push_back(FVertex(
				Position,
				XMFLOAT4(Colors::Gray),
				PlaneNormal, // 法线   // 法線
				XMFLOAT2(VerticalAverageSubdivision * i, HorizontalAverageSubdivision * j))); // UV自动展开  // UVを展開
			MeshData.VertexData.back().UTangent = PlaneTangent;
		}
	}

	// 绘制index
	// インデックスを描画
	for (uint32_t i = 0; i < InHeightSubdivide - 1; ++i)
	{
		for (uint32_t j = 0; j < InWidthSubdivide - 1; ++j)
		{
			// 三角形1
			MeshData.IndexData.push_back((i + 1) * InWidthSubdivide + j);
			MeshData.IndexData.push_back(i * InWidthSubdivide + j + 1);
			MeshData.IndexData.push_back(i * InWidthSubdivide + j);

			// 三角形2
			MeshData.IndexData.push_back((i + 1) * InWidthSubdivide + j + 1);
			MeshData.IndexData.push_back(i * InWidthSubdivide + j + 1);
			MeshData.IndexData.push_back((i + 1) * InWidthSubdivide + j);
		}
	}
}

void CPlaneMeshComponent::BuildKey(
	size_t& OutHashKey,
	float InHeight,
	float InWidth,
	uint32_t InHeightSubdivide,
	uint32_t InWidthSubdivide,
	const fvector_3d& InGenerateRotation)
{
	std::hash<float> FloatHash;
	std::hash<int> IntHash;

	OutHashKey = 6;
	OutHashKey += FloatHash(InHeight);
	OutHashKey += FloatHash(InWidth);

	OutHashKey += IntHash._Do_hash(InHeightSubdivide);
	OutHashKey += IntHash._Do_hash(InWidthSubdivide);
	OutHashKey += FloatHash((float)InGenerateRotation.x);
	OutHashKey += FloatHash((float)InGenerateRotation.y);
	OutHashKey += FloatHash((float)InGenerateRotation.z);
}
