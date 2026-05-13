#include "SpriteComponent.h"

#include "../../Mesh/Core/Material/Material.h"
#include "../../Core/Construction/MacroConstruction.h"
#include "../../Math/EngineMath.h"
#include "../../Rendering/Core/DirectX/RenderingPipeline/Geometry/GeometryMap.h"
#include "../../Rendering/Core/Sprite/SpriteAtlasManager.h"

CSpriteComponent::CSpriteComponent()
	: TintColor(1.f, 1.f, 1.f, 1.f)
	, SpriteScale(1.f, 1.f, 1.f)
	, PixelsPerUnit(100.f)
	, Opacity(1.f)
	, bAutoSizeToFrame(true)
{
	SetMeshRenderLayerType(EMeshRenderLayerType::RENDERLAYER_TRANSPARENT);
	SetCastShadow(false);
}


void CSpriteComponent::Init()
{
	CMeshComponent::Init();
	ApplySpriteState();
}


void CSpriteComponent::SetScale(const fvector_3d& InNewScale)
{
	SpriteScale = InNewScale;
	ApplyDisplayScale();
}

void CSpriteComponent::CreateMesh(FMeshRenderingData& MeshData, float InHeight, float InWidth)
{
	const float HalfHeight = 0.5f * InHeight;
	const float HalfWidth = 0.5f * InWidth;

	MeshData.VertexData.push_back(FVertex(
		XMFLOAT3(-HalfWidth, -HalfHeight, 0.f),
		XMFLOAT4(Colors::White),
		XMFLOAT3(0.f, 0.f, 1.f),
		XMFLOAT2(0.f, 1.f)));
	MeshData.VertexData.push_back(FVertex(
		XMFLOAT3(-HalfWidth, HalfHeight, 0.f),
		XMFLOAT4(Colors::White),
		XMFLOAT3(0.f, 0.f, 1.f),
		XMFLOAT2(0.f, 0.f)));
	MeshData.VertexData.push_back(FVertex(
		XMFLOAT3(HalfWidth, HalfHeight, 0.f),
		XMFLOAT4(Colors::White),
		XMFLOAT3(0.f, 0.f, 1.f),
		XMFLOAT2(1.f, 0.f)));
	MeshData.VertexData.push_back(FVertex(
		XMFLOAT3(HalfWidth, -HalfHeight, 0.f),
		XMFLOAT4(Colors::White),
		XMFLOAT3(0.f, 0.f, 1.f),
		XMFLOAT2(1.f, 1.f)));

	MeshData.IndexData.push_back(0);
	MeshData.IndexData.push_back(1);
	MeshData.IndexData.push_back(2);

	MeshData.IndexData.push_back(0);
	MeshData.IndexData.push_back(2);
	MeshData.IndexData.push_back(3);
}

void CSpriteComponent::BuildKey(size_t& OutHashKey, float InHeight, float InWidth)
{
	std::hash<float> FloatHash;
	OutHashKey = 10001;
	OutHashKey += FloatHash(InHeight);
	OutHashKey += FloatHash(InWidth);
}

void CSpriteComponent::BuildSprite(float InHeight, float InWidth)
{
	CREATE_RENDER_DATA_BY_COMPONENT(CSpriteComponent, this, InHeight, InWidth);
	ApplySpriteState();
}

// 从xml数据加载图集Atlas，并绑定到当前Sprite
// XMLデータからatlasを読み込み、現在のスプライトに設定する
bool CSpriteComponent::LoadAtlas(const string& InAtlasName, const string& InMetadataFilename, const string& InTextureKey)
{
	shared_ptr<CSpriteAtlas> LoadedAtlas =
		CSpriteAtlasManager::LoadAtlas(InAtlasName, InMetadataFilename, InTextureKey);

	return SetAtlas(LoadedAtlas);
}

// 设置atlas，修正当前帧并刷新显示状态
// atlasを設定し、現在のフレームを修正して表示状態を更新する
bool CSpriteComponent::SetAtlas(const shared_ptr<CSpriteAtlas>& InAtlas)
{
	if (InAtlas == nullptr)
	{
		return false;
	}

	Atlas = InAtlas;

	if (CurrentFrameName.empty())
	{
		if (const FSpriteAtlasFrame* FirstFrame = Atlas->GetFirstFrame())
		{
			CurrentFrameName = FirstFrame->Name;
		}
	}
	else if (Atlas->FindFrame(CurrentFrameName) == nullptr)
	{
		if (const FSpriteAtlasFrame* FirstFrame = Atlas->GetFirstFrame())
		{
			CurrentFrameName = FirstFrame->Name;
		}
	}

	ApplySpriteState();
	return !CurrentFrameName.empty();
}

// 切换到指定帧，并同步材质、UV 与显示尺寸
// 指定フレームに切り替え, material, UV、サイズを修正する
bool CSpriteComponent::SetFrame(const string& InFrameName)
{
	if (Atlas == nullptr)
	{
		return false;
	}

	if (Atlas->FindFrame(InFrameName) == nullptr)
	{
		return false;
	}

	CurrentFrameName = InFrameName;
	ApplySpriteState();
	return true;
}

void CSpriteComponent::SetTintColor(const fvector_4d& InTintColor)
{
	TintColor = InTintColor;
	ApplyMaterialSettings();
}

void CSpriteComponent::SetOpacity(float InOpacity)
{
	Opacity = min(max(InOpacity, 0.f), 1.f);
	ApplyMaterialSettings();
}

void CSpriteComponent::SetPixelsPerUnit(float InPixelsPerUnit)
{
	PixelsPerUnit = max(0.001f, InPixelsPerUnit);
	ApplyDisplayScale();
}

void CSpriteComponent::SetAutoSizeToFrame(bool bInAutoSizeToFrame)
{
	bAutoSizeToFrame = bInAutoSizeToFrame;
	ApplyDisplayScale();
}

const FSpriteAtlasFrame* CSpriteComponent::GetCurrentFrame() const
{
	if (Atlas == nullptr || CurrentFrameName.empty())
	{
		return nullptr;
	}

	return Atlas->FindFrame(CurrentFrameName);
}

// 统一更新 sprite 的材质、UV 与显示缩放
// Spriteのマテリアル、UV、表示サイズを一括更新する
void CSpriteComponent::ApplySpriteState()
{
	ApplyMaterialSettings();
	ApplyTextureTransform();
	ApplyDisplayScale();
}

// 写入材质　
// マテリアルに反応する
void CSpriteComponent::ApplyMaterialSettings()
{
	SetMaterialCount(1);

	CMaterial* Material = GetMaterialBySlot(0);
	if (Material == nullptr)
	{
		return;
	}

	fvector_4d BaseColor = TintColor;
	BaseColor.w = Opacity;

	Material->SetBaseColor(BaseColor);
	Material->SetMaterialType(EMaterialType::BaseColor);

	if (Atlas != nullptr)
	{
		Material->SetBaseColor(Atlas->GetTextureKey());
	}
}

// 将当前帧对应的 UV 变换写回渲染数据
// 現在のフレームに対応するUV変換をレンダリングデータに書き戻す
void CSpriteComponent::ApplyTextureTransform()
{
	const XMFLOAT4X4 TextureTransform = BuildTextureTransformForCurrentFrame();

	FGeometry::FindRenderingDatas(
		[&](shared_ptr<FRenderingData>& InRenderingData)->EFindValueType
		{
			if (InRenderingData->Mesh == this)
			{
				InRenderingData->TextureTransform = TextureTransform;
			}

			return EFindValueType::TYPE_IN_PROGRAM;
		});
}

// 根据像素尺寸与基础缩放计算最终显示大小
// ピクセルサイズとスケーリングに基づいて、最終的なサイズを計算する
void CSpriteComponent::ApplyDisplayScale()
{
	fvector_3d FinalScale = SpriteScale;

	if (bAutoSizeToFrame)
	{
		if (const FSpriteAtlasFrame* Frame = GetCurrentFrame())
		{
			FinalScale.x *= Frame->Width / PixelsPerUnit;
			FinalScale.y *= Frame->Height / PixelsPerUnit;
		}
	}

	CMeshComponent::SetScale(FinalScale);
}

// 从 atlas 中生成当前帧的纹理变换矩阵
// atlas から現在のフレームのテクスチャ変換行列を生成する
XMFLOAT4X4 CSpriteComponent::BuildTextureTransformForCurrentFrame() const
{
	XMFLOAT4X4 TextureTransform = EngineMath::IdentityMatrix4x4();

	if (Atlas == nullptr || CurrentFrameName.empty())
	{
		return TextureTransform;
	}

	Atlas->BuildFrameTextureTransform(CurrentFrameName, TextureTransform);
	return TextureTransform;
}
