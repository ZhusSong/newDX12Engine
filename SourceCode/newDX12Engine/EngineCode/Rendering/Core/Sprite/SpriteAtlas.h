#pragma once

#include "../../../EngineMinimal.h"


struct FSpriteAtlasFrame
{
	FSpriteAtlasFrame();

	string Name;

	float X;
	float Y;
	float Width;
	float Height;
};

class CSpriteAtlas
{
public:
	CSpriteAtlas();

	bool LoadFromFile(const string& InMetadataFilename, const string& InOverrideTextureKey = string());

	const FSpriteAtlasFrame* FindFrame(const string& InFrameName) const;
	const FSpriteAtlasFrame* GetFirstFrame() const;
	bool BuildFrameTextureTransform(const string& InFrameName, XMFLOAT4X4& OutTextureTransform) const;
	bool HasValidAtlasSize() const;

	const string& GetAtlasName() const { return AtlasName; }
	const string& GetTextureKey() const { return TextureKey; }

	float GetAtlasWidth() const { return AtlasWidth; }
	float GetAtlasHeight() const { return AtlasHeight; }

	void SetAtlasName(const string& InAtlasName) { AtlasName = InAtlasName; }

protected:
	bool ParseAtlasRoot(TiXmlElement* InRootElement);
	bool ParseFrameElement(TiXmlElement* InFrameElement);

	static bool ReadFloatAttribute(TiXmlElement* InElement, const char* InAttributeName, float& OutValue);
	static bool ReadStringAttribute(TiXmlElement* InElement, const char* InAttributeName, string& OutValue);
	static string BuildStemFromPath(const string& InFilename);

protected:
	string AtlasName;
	string MetadataFilename;
	string TextureKey;

	float AtlasWidth;
	float AtlasHeight;

	map<string, FSpriteAtlasFrame> Frames;
};
