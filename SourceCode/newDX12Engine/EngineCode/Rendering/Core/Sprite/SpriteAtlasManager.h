#pragma once

#include "SpriteAtlas.h"


class CSpriteAtlasManager
{
public:
	static shared_ptr<CSpriteAtlas> LoadAtlas(
		const string& InAtlasName,
		const string& InMetadataFilename,
		const string& InTextureKey = string());

	static bool GenerateGridAtlasXML(
		const string& InTextureFilename,
		const string& InOutputXMLFilename,
		int InColumns,
		int InRows,
		const string& InFramePrefix = string(),
		int InFrameCount = -1);

	static shared_ptr<CSpriteAtlas> FindAtlas(const string& InAtlasName);
	static void RegisterAtlas(const shared_ptr<CSpriteAtlas>& InAtlas);
	static void Clear();

protected:
	static bool ResolveImageSize(const string& InTextureFilename, UINT& OutWidth, UINT& OutHeight);
	static string ResolveAssetFilename(const string& InFilename);
	static string BuildDefaultXMLFilename(const string& InTextureFilename);
	static string BuildDefaultFramePrefix(const string& InTextureFilename);

	static map<string, shared_ptr<CSpriteAtlas>> AtlasMapping;
};
