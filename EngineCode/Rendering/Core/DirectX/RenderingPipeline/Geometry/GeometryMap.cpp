#include "GeometryMap.h"
#include "../../../Buffer/ConstructBuffer.h"
#include "../../../../../Mesh/Core/ObjectTransform.h"
#include "../../../../../Core/Viewport/ViewportTransformation.h"
#include "../../../../../Mesh/Core/Mesh.h"

bool FGeometry::bRenderingDataExistence(GMesh* InKey)
{
	for (auto& Tmp : DescribeMeshRenderingData)
	{
		if (Tmp.Mesh == InKey)
		{
			return true;
		}
	}

	return false;
}
void FGeometry::BuildMesh(GMesh* InMesh, const FMeshRenderingData& MeshData)
{
	// 判断当前模型数据是否已添加过
	if (!bRenderingDataExistence(InMesh))
	{
		DescribeMeshRenderingData.push_back(FRenderingData());

		// 获取刚刚添加的数据
		FRenderingData& InRenderingData = DescribeMeshRenderingData[DescribeMeshRenderingData.size() - 1];

		InRenderingData.Mesh = InMesh;

		// 记录数据尺寸
		InRenderingData.IndexSize = MeshData.IndexData.size();
		InRenderingData.VertexSize = MeshData.VertexData.size();

		// 记录偏移位置
		InRenderingData.IndexOffsetPosition = MeshRenderingData.IndexData.size();
		InRenderingData.VertexOffsetPosition = MeshRenderingData.VertexData.size();


		// 插入渲染数据
		// 添加索引数据
		MeshRenderingData.IndexData.insert(
			MeshRenderingData.IndexData.end(), // 位置为当前列表尾部
			MeshData.IndexData.begin(),        // 本次数据头部
			MeshData.IndexData.end());         // 本次数据尾部

		// 添加顶点数据
		MeshRenderingData.VertexData.insert(
			MeshRenderingData.VertexData.end(),
			MeshData.VertexData.begin(),
			MeshData.VertexData.end());
	}
}

// 初始化几何体数据
FGeometryMap::FGeometryMap()
{
	Geometrys.insert(pair<int, FGeometry>(0, FGeometry()));
}


void FGeometryMap::PreDraw(float DeltaTime)
{
	DescriptorHeap.PreDraw(DeltaTime);
}

void FGeometryMap::Draw(float DeltaTime)
{
	//葷ﾈｾﾊﾓｿﾚ
	DrawViewport(DeltaTime);

	//葷ﾈｾﾄ｣ﾐﾍ
	DrawMesh(DeltaTime);
}

void FGeometryMap::PostDraw(float DeltaTime)
{

}

void FGeometryMap::UpdateCalculations(float DeltaTime, const FViewportInfo& ViewportInfo)
{
	XMMATRIX ViewMatrix = XMLoadFloat4x4(&ViewportInfo.ViewMatrix);
	XMMATRIX ProjectMatrix = XMLoadFloat4x4(&ViewportInfo.ProjectMatrix);

	for (auto& Tmp : Geometrys)//ﾔﾝﾊｱﾏﾈﾕ篥ｴﾐｴ
	{
		for (size_t i = 0; i < Tmp.second.DescribeMeshRenderingData.size(); i++)
		{
			FRenderingData& InRenderingData = Tmp.second.DescribeMeshRenderingData[i];

			//ｹｹﾔ・｣ﾐﾍｵﾄworld
			{
				XMFLOAT3& Position = InRenderingData.Mesh->GetPosition();
				fvector_3d Scale = InRenderingData.Mesh->GetScale();

				XMFLOAT3 RightVector = InRenderingData.Mesh->GetRightVector();
				XMFLOAT3 UPVector = InRenderingData.Mesh->GetUPVector();
				XMFLOAT3 ForwardVector = InRenderingData.Mesh->GetForwardVector();

				InRenderingData.WorldMatrix = {
					RightVector.x * Scale.x,	UPVector.x,				ForwardVector.x ,			0.f,
					RightVector.y,				UPVector.y * Scale.x,	ForwardVector.y,			0.f,
					RightVector.z,				UPVector.z ,			ForwardVector.z * Scale.x,	0.f,
					Position.x,					Position.y,				Position.z,					1.f };
			}

			//ｸ・ﾂﾄ｣ﾐﾍﾎｻﾖﾃ
			XMMATRIX ATRIXWorld = XMLoadFloat4x4(&InRenderingData.WorldMatrix);

			FObjectTransform ObjectTransformation;
			XMStoreFloat4x4(&ObjectTransformation.World, XMMatrixTranspose(ATRIXWorld));
			ObjectConstantBufferViews.Update(i, &ObjectTransformation);
		}
	}

	//ｸ・ﾂﾊﾓｿﾚ
	XMMATRIX ViewProject = XMMatrixMultiply(ViewMatrix, ProjectMatrix);
	FViewportTransformation ViewportTransformation;
	XMStoreFloat4x4(&ViewportTransformation.ViewProjectionMatrix, XMMatrixTranspose(ViewProject));

	ViewportConstantBufferViews.Update(0, &ViewportTransformation);
}

void FGeometryMap::BuildMesh(GMesh* InMesh, const FMeshRenderingData& MeshData)
{
	FGeometry& Geometry = Geometrys[0];

	Geometry.BuildMesh(InMesh, MeshData);
}

void FGeometryMap::Build()
{
	//ｹｹｽｨﾄ｣ﾐﾍ
	for (auto& Tmp : Geometrys)
	{
		Tmp.second.Build();
	}
}

void FGeometryMap::BuildDescriptorHeap()
{
	//+1ﾉ耘・
	DescriptorHeap.Build(GetDrawObjectNumber() + 1);
}

void FGeometryMap::BuildConstantBuffer()
{
	//ｴｴｽｨｳ｣ﾁｿｻｺｳ衂・
	ObjectConstantBufferViews.CreateConstant(sizeof(FObjectTransform), GetDrawObjectNumber());

	//Handle
	CD3DX12_CPU_DESCRIPTOR_HANDLE DesHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(GetHeap()->GetCPUDescriptorHandleForHeapStart());

	//ｹｹｽｨｳ｣ﾁｿｻｺｳ衂・
	ObjectConstantBufferViews.BuildConstantBuffer(DesHandle, GetDrawObjectNumber());
}

UINT FGeometryMap::GetDrawObjectNumber()
{
	return Geometrys[0].GetDrawObjectNumber();
}

void FGeometryMap::BuildViewportConstantBufferView()
{
	//ｴｴｽｨｳ｣ﾁｿｻｺｳ衂・
	ViewportConstantBufferViews.CreateConstant(sizeof(FViewportTransformation), 1);

	//Handle
	CD3DX12_CPU_DESCRIPTOR_HANDLE DesHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(DescriptorHeap.GetHeap()->GetCPUDescriptorHandleForHeapStart());

	//ｹｹｽｨｳ｣ﾁｿｻｺｳ衂・
	ViewportConstantBufferViews.BuildConstantBuffer(DesHandle, 1, GetDrawObjectNumber());
}

void FGeometryMap::DrawViewport(float DeltaTime)
{
	UINT DescriptorOffset = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto DesHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GetHeap()->GetGPUDescriptorHandleForHeapStart());
	DesHandle.Offset(GetDrawObjectNumber(), DescriptorOffset);

	GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(1, DesHandle);
}

void FGeometryMap::DrawMesh(float DeltaTime)
{
	UINT DescriptorOffset = GetD3dDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//ﾄ｣ﾐﾍｹｹｽｨ
	for (auto& Tmp : Geometrys)
	{
		D3D12_VERTEX_BUFFER_VIEW VBV = Tmp.second.GetVertexBufferView();
		D3D12_INDEX_BUFFER_VIEW IBV = Tmp.second.GetIndexBufferView();

		for (int i = 0; i < Tmp.second.DescribeMeshRenderingData.size(); i++)
		{
			auto DesHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GetHeap()->GetGPUDescriptorHandleForHeapStart());

			FRenderingData& InRenderingData = Tmp.second.DescribeMeshRenderingData[i];

			GetGraphicsCommandList()->IASetIndexBuffer(&IBV);

			//ｰｨ葷ﾈｾﾁｮﾏﾟﾉﾏｵﾄﾊ菠・ﾛ｣ｬｿﾉﾒﾔﾔﾚﾊ菠・ｰﾅ菷ﾗｶﾎｴｫﾈ・･ｵ飜ｾﾝ
			GetGraphicsCommandList()->IASetVertexBuffers(
				0,//ﾆｼﾊ菠・ﾛ 0-15 
				1,//k k+1 ... k+n-1 
				&VBV);

			//ｶｨﾒ衾ﾒﾃﾇﾒｪｻ贍ﾆｵﾄﾄﾄﾖﾖﾍｼﾔｪ ｵ・ﾏﾟ ﾃ・
			GetGraphicsCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//ﾆｼｵﾘﾖｷﾆｫﾒﾆ
			DesHandle.Offset(i, DescriptorOffset);
			GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(0, DesHandle);

			//ﾕ贏ｵﾄｻ贍ﾆ
			GetGraphicsCommandList()->DrawIndexedInstanced(
				InRenderingData.IndexSize,//ｶ･ｵ飜ﾁｿ
				1,//ｻ贍ﾆﾊｵﾀﾊﾁｿ
				InRenderingData.IndexOffsetPosition,//ｶ･ｵ羹ｺｳ衂ﾚﾒｻｸｻｻ贍ﾆｵﾄﾋ
				InRenderingData.VertexOffsetPosition,//GPU ｴﾓﾋｻｺｳ衂ﾁﾈ｡ｵﾄｵﾚﾒｻｸｵﾄﾎｻﾖﾃ｡｣
				0);//ﾔﾚｴﾓｶ･ｵ羹ｺｳ衂ﾁﾈ｡ﾃｿｸｵﾀﾊｾﾝﾖｮﾇｰﾌ晴ﾓｵｽﾃｿｸｵﾄﾖｵ｡｣
		}
	}
}



