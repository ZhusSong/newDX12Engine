#include "SpriteAtlasManager.h"

#include "../DX12EngineGeneralSetting/Core/Path/EnginePath.h"

#include <wincodec.h>
#include <fstream>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "windowscodecs.lib")

namespace
{
	static string NormalizePathSeparators(string InPath)
	{
		std::replace(InPath.begin(), InPath.end(), '\\', '/');
		return InPath;
	}

	static string BuildStemFromFilename(const string& InFilename)
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

	static string BuildFilenameOnly(const string& InFilename)
	{
		string NormalizedFilename = NormalizePathSeparators(InFilename);

		const size_t SlashPosition = NormalizedFilename.find_last_of('/');
		return SlashPosition == string::npos
			? NormalizedFilename
			: NormalizedFilename.substr(SlashPosition + 1);
	}
}

map<string, shared_ptr<CSpriteAtlas>> CSpriteAtlasManager::AtlasMapping;

shared_ptr<CSpriteAtlas> CSpriteAtlasManager::LoadAtlas(
	const string& InAtlasName,
	const string& InMetadataFilename,
	const string& InTextureKey)
{
	shared_ptr<CSpriteAtlas> Atlas = std::make_shared<CSpriteAtlas>();
	Atlas->SetAtlasName(InAtlasName);
	if (!Atlas->LoadFromFile(InMetadataFilename, InTextureKey))
	{
		return nullptr;
	}

	RegisterAtlas(Atlas);
	return Atlas;
}

// 生成 atlas xml
// アトラスXMLを作成する
bool CSpriteAtlasManager::GenerateGridAtlasXML(
	const string& InTextureFilename,
	const string& InOutputXMLFilename,
	int InColumns,
	int InRows,
	const string& InFramePrefix,
	int InFrameCount)
{
	if (InColumns <= 0 || InRows <= 0)
	{
		return false;
	}

	// 先读取图集整体尺寸，再按列行数均分出每个子帧尺寸
	// まず画像全体のサイズを読み取り、その後、列数と行数に応じて各サブフレームのサイズを均等に分割する
	UINT AtlasWidth = 0;
	UINT AtlasHeight = 0;
	if (!ResolveImageSize(InTextureFilename, AtlasWidth, AtlasHeight))
	{
		return false;
	}

	if (AtlasWidth == 0 || AtlasHeight == 0)
	{
		return false;
	}

	if ((AtlasWidth % (UINT)InColumns) != 0 || (AtlasHeight % (UINT)InRows) != 0)
	{
		return false;
	}

	const UINT CellWidth = AtlasWidth / (UINT)InColumns;
	const UINT CellHeight = AtlasHeight / (UINT)InRows;
	if (CellWidth == 0 || CellHeight == 0)
	{
		return false;
	}
	const int MaxFrameCount = InColumns * InRows;
	const int FrameCount = InFrameCount > 0 ? min(InFrameCount, MaxFrameCount) : MaxFrameCount;

	const string OutputXMLFilename = ResolveAssetFilename(
		InOutputXMLFilename.empty() ? BuildDefaultXMLFilename(InTextureFilename) : InOutputXMLFilename);
	const string FramePrefix = InFramePrefix.empty() ? BuildDefaultFramePrefix(InTextureFilename) : InFramePrefix;

	const string ImageFilename = BuildFilenameOnly(InTextureFilename);

	std::ofstream OutputFile(OutputXMLFilename.c_str(), std::ios::binary | std::ios::trunc);
	if (!OutputFile.is_open())
	{
		return false;
	}

	OutputFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	OutputFile << "<TextureAtlas imagePath=\"" << ImageFilename << "\" width=\"" << AtlasWidth << "\" height=\"" << AtlasHeight << "\">\n";

	// 按行优先顺序写出每一帧的矩形区域
	// 各フレームの矩形領域を行優先順に記述する
	for (int FrameIndex = 0; FrameIndex < FrameCount; ++FrameIndex)
	{
		const int ColumnIndex = FrameIndex % InColumns;
		const int RowIndex = FrameIndex / InColumns;

		std::ostringstream FrameNameStream;
		FrameNameStream << FramePrefix << "_" << std::setfill('0') << std::setw(2) << FrameIndex;

		OutputFile
			<< "  <SubTexture name=\"" << FrameNameStream.str()
			<< "\" x=\"" << (ColumnIndex * (int)CellWidth)
			<< "\" y=\"" << (RowIndex * (int)CellHeight)
			<< "\" width=\"" << CellWidth
			<< "\" height=\"" << CellHeight
			<< "\" />\n";
	}

	OutputFile << "</TextureAtlas>\n";
	return true;
}

shared_ptr<CSpriteAtlas> CSpriteAtlasManager::FindAtlas(const string& InAtlasName)
{
	auto FindAtlasIterator = AtlasMapping.find(InAtlasName);
	if (FindAtlasIterator != AtlasMapping.end())
	{
		return FindAtlasIterator->second;
	}

	return nullptr;
}

void CSpriteAtlasManager::RegisterAtlas(const shared_ptr<CSpriteAtlas>& InAtlas)
{
	if (InAtlas == nullptr || InAtlas->GetAtlasName().empty())
	{
		return;
	}

	AtlasMapping[InAtlas->GetAtlasName()] = InAtlas;
}

void CSpriteAtlasManager::Clear()
{
	AtlasMapping.clear();
}

// 读取图片尺寸，生成图集xml
// 画像のサイズを読み取り、 atlasのXMLを生成する
bool CSpriteAtlasManager::ResolveImageSize(const string& InTextureFilename, UINT& OutWidth, UINT& OutHeight)
{
	OutWidth = 0;
	OutHeight = 0;

	const string ResolvedFilename = ResolveAssetFilename(InTextureFilename);
	if (ResolvedFilename.empty())
	{
		return false;
	}

	wchar_t TextureFilename[1024] = { 0 };
	char_to_wchar_t(TextureFilename, 1024, ResolvedFilename.c_str());

	ComPtr<IWICImagingFactory> ImagingFactory;
	if (FAILED(CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&ImagingFactory))))
	{
		return false;
	}

	ComPtr<IWICBitmapDecoder> BitmapDecoder;
	if (FAILED(ImagingFactory->CreateDecoderFromFilename(
		TextureFilename,
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		BitmapDecoder.GetAddressOf())))
	{
		return false;
	}

	ComPtr<IWICBitmapFrameDecode> BitmapFrame;
	if (FAILED(BitmapDecoder->GetFrame(0, BitmapFrame.GetAddressOf())))
	{
		return false;
	}

	return SUCCEEDED(BitmapFrame->GetSize(&OutWidth, &OutHeight));
}

// 将相对资源路径转换为引擎资源目录下的绝对路径
// リソースの相対パスを、エンジンリソースディレクトリ内の絶対パスに変換する
string CSpriteAtlasManager::ResolveAssetFilename(const string& InFilename)
{
	if (InFilename.empty())
	{
		return string();
	}

	string NormalizedFilename = NormalizePathSeparators(InFilename);
	if (NormalizedFilename.find(':') != string::npos)
	{
		return NormalizedFilename;
	}

	const string AssetRoot =
		FEnginePathHelper::RelativeToAbsolutePath(
			FEnginePathHelper::GetEngineAssetPath());

	return NormalizePathSeparators(AssetRoot + "/" + NormalizedFilename);
}

// 为图集贴图生成默认的 xml 输出文件名
// atlasの画像に対して、XML出力ファイル名を生成する
string CSpriteAtlasManager::BuildDefaultXMLFilename(const string& InTextureFilename)
{
	string NormalizedFilename = NormalizePathSeparators(InTextureFilename);

	const size_t DotPosition = NormalizedFilename.find_last_of('.');
	if (DotPosition != string::npos)
	{
		NormalizedFilename = NormalizedFilename.substr(0, DotPosition);
	}

	return NormalizedFilename + ".xml";
}

string CSpriteAtlasManager::BuildDefaultFramePrefix(const string& InTextureFilename)
{
	return BuildStemFromFilename(InTextureFilename);
}
