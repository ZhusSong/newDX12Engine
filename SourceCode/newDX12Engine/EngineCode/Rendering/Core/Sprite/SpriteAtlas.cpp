#include "SpriteAtlas.h"

#include "../../../Math/EngineMath.h"

#include <fstream>

namespace
{
	static string NormalizePathSeparators(string InPath)
	{
		std::replace(InPath.begin(), InPath.end(), '\\', '/');
		return InPath;
	}

	static bool FileExists(const string& InFilename)
	{
		std::ifstream FileStream(InFilename.c_str(), std::ios::binary);
		return FileStream.good();
	}

	static string ResolveMetadataFilename(const string& InMetadataFilename)
	{
		if (InMetadataFilename.empty())
		{
			return string();
		}

		if (FileExists(InMetadataFilename))
		{
			return NormalizePathSeparators(InMetadataFilename);
		}

		const string AssetRoot =
			FEnginePathHelper::RelativeToAbsolutePath(
				FEnginePathHelper::GetEngineAssetPath());

		const string AssetFilename = NormalizePathSeparators(AssetRoot + "/" + InMetadataFilename);
		if (FileExists(AssetFilename))
		{
			return AssetFilename;
		}

		return NormalizePathSeparators(InMetadataFilename);
	}

	static string ToLowerString(string InValue)
	{
		for (char& Character : InValue)
		{
			Character = (char)std::tolower((unsigned char)Character);
		}

		return InValue;
	}
}

FSpriteAtlasFrame::FSpriteAtlasFrame()
	: X(0.f)
	, Y(0.f)
	, Width(0.f)
	, Height(0.f)
{
}

CSpriteAtlas::CSpriteAtlas()
	: AtlasWidth(0.f)
	, AtlasHeight(0.f)
{
}

// 从xml数据中读取atlas尺寸、贴图key与帧列表
// XMLからatlasのサイズ、テクスチャキー、フレームリストを読み込む
bool CSpriteAtlas::LoadFromFile(const string& InMetadataFilename, const string& InOverrideTextureKey)
{
	Frames.clear();
	TextureKey.clear();
	AtlasWidth = 0.f;
	AtlasHeight = 0.f;

	MetadataFilename = ResolveMetadataFilename(InMetadataFilename);
	if (MetadataFilename.empty())
	{
		return false;
	}

	TiXmlDocument Document(MetadataFilename.c_str());
	if (!Document.LoadFile())
	{
		return false;
	}

	TiXmlElement* RootElement = Document.RootElement();
	if (RootElement == nullptr)
	{
		return false;
	}

	if (!ParseAtlasRoot(RootElement))
	{
		return false;
	}

	if (!InOverrideTextureKey.empty())
	{
		TextureKey = InOverrideTextureKey;
	}

	if (AtlasName.empty())
	{
		AtlasName = BuildStemFromPath(MetadataFilename);
	}

	if (TextureKey.empty())
	{
		TextureKey = AtlasName;
	}

	return !Frames.empty() && HasValidAtlasSize();
}

const FSpriteAtlasFrame* CSpriteAtlas::FindFrame(const string& InFrameName) const
{
	auto FindFrameIterator = Frames.find(InFrameName);
	if (FindFrameIterator != Frames.end())
	{
		return &FindFrameIterator->second;
	}

	return nullptr;
}

const FSpriteAtlasFrame* CSpriteAtlas::GetFirstFrame() const
{
	if (!Frames.empty())
	{
		return &Frames.begin()->second;
	}

	return nullptr;
}

// 根据帧矩形生成最终采样用的 UV 变换矩阵
// フレーム矩形に基づいて、最終的なサンプリングに使用するUV変換行列を生成する
bool CSpriteAtlas::BuildFrameTextureTransform(const string& InFrameName, XMFLOAT4X4& OutTextureTransform) const
{
	OutTextureTransform = EngineMath::IdentityMatrix4x4();

	const FSpriteAtlasFrame* Frame = FindFrame(InFrameName);
	if (Frame == nullptr || !HasValidAtlasSize())
	{
		return false;
	}

	// 向帧区域内部收缩0.5像素，减少采样串到相邻帧的概率
	// フレーム領域の内側に0.5ピクセル収縮させ、隣接するフレームへのサンプリングが波及する確率を低減する
	const float HalfTexelU = AtlasWidth > 0.f ? (0.5f / AtlasWidth) : 0.f;
	const float HalfTexelV = AtlasHeight > 0.f ? (0.5f / AtlasHeight) : 0.f;

	const float StartU = (Frame->X / AtlasWidth) + HalfTexelU;
	const float StartV = (Frame->Y / AtlasHeight) + HalfTexelV;
	const float EndU = ((Frame->X + Frame->Width) / AtlasWidth) - HalfTexelU;
	const float EndV = ((Frame->Y + Frame->Height) / AtlasHeight) - HalfTexelV;

	OutTextureTransform._11 = max(EndU - StartU, 0.f);
	OutTextureTransform._22 = max(EndV - StartV, 0.f);
	OutTextureTransform._41 = StartU;
	OutTextureTransform._42 = StartV;

	return true;
}

bool CSpriteAtlas::HasValidAtlasSize() const
{
	return AtlasWidth > 0.f && AtlasHeight > 0.f;
}

// 解析atlas根节点，并扫描其中的帧子节点
// アトラスのルートノードを解析し、その中のフレーム子ノードを走査する
bool CSpriteAtlas::ParseAtlasRoot(TiXmlElement* InRootElement)
{
	if (InRootElement == nullptr)
	{
		return false;
	}

	ReadStringAttribute(InRootElement, "imagePath", TextureKey);
	if (TextureKey.empty())
	{
		ReadStringAttribute(InRootElement, "texturePath", TextureKey);
	}

	if (!TextureKey.empty())
	{
		TextureKey = BuildStemFromPath(TextureKey);
	}

	ReadFloatAttribute(InRootElement, "width", AtlasWidth);
	ReadFloatAttribute(InRootElement, "height", AtlasHeight);

	for (TiXmlElement* ChildElement = InRootElement->FirstChildElement();
		ChildElement != nullptr;
		ChildElement = ChildElement->NextSiblingElement())
	{
		const string ElementName = ToLowerString(ChildElement->Value());
		if (ElementName == "subtexture" || ElementName == "sprite")
		{
			ParseFrameElement(ChildElement);
			continue;
		}

		if (ElementName == "meta")
		{
			if (AtlasWidth <= 0.f)
			{
				ReadFloatAttribute(ChildElement, "width", AtlasWidth);
			}

			if (AtlasHeight <= 0.f)
			{
				ReadFloatAttribute(ChildElement, "height", AtlasHeight);
			}
		}
	}

	return true;
}

// 解析单个帧节点，提取名称与矩形区域
// 個々のフレームノードを解析し、名前と矩形領域を抽出する
bool CSpriteAtlas::ParseFrameElement(TiXmlElement* InFrameElement)
{
	if (InFrameElement == nullptr)
	{
		return false;
	}

	FSpriteAtlasFrame Frame;
	if (!ReadStringAttribute(InFrameElement, "name", Frame.Name))
	{
		ReadStringAttribute(InFrameElement, "n", Frame.Name);
	}

	if (Frame.Name.empty())
	{
		return false;
	}

	ReadFloatAttribute(InFrameElement, "x", Frame.X);
	ReadFloatAttribute(InFrameElement, "y", Frame.Y);

	if (!ReadFloatAttribute(InFrameElement, "width", Frame.Width))
	{
		ReadFloatAttribute(InFrameElement, "w", Frame.Width);
	}

	if (!ReadFloatAttribute(InFrameElement, "height", Frame.Height))
	{
		ReadFloatAttribute(InFrameElement, "h", Frame.Height);
	}

	Frames[Frame.Name] = Frame;
	return true;
}

bool CSpriteAtlas::ReadFloatAttribute(TiXmlElement* InElement, const char* InAttributeName, float& OutValue)
{
	if (InElement == nullptr || InAttributeName == nullptr)
	{
		return false;
	}

	const char* AttributeValue = InElement->Attribute(InAttributeName);
	if (AttributeValue == nullptr)
	{
		return false;
	}

	OutValue = (float)atof(AttributeValue);
	return true;
}

bool CSpriteAtlas::ReadStringAttribute(TiXmlElement* InElement, const char* InAttributeName, string& OutValue)
{
	if (InElement == nullptr || InAttributeName == nullptr)
	{
		return false;
	}

	const char* AttributeValue = InElement->Attribute(InAttributeName);
	if (AttributeValue == nullptr)
	{
		return false;
	}

	OutValue = AttributeValue;
	return true;
}

string CSpriteAtlas::BuildStemFromPath(const string& InFilename)
{
	string NormalizedFilename = NormalizePathSeparators(InFilename);

	const size_t SlashPosition = NormalizedFilename.find_last_of('/');
	string Stem = SlashPosition == string::npos
		? NormalizedFilename
		: NormalizedFilename.substr(SlashPosition + 1);

	const size_t DotPosition = Stem.find_last_of('.');
	if (DotPosition != string::npos)
	{
		Stem = Stem.substr(0, DotPosition);
	}

	return Stem;
}
