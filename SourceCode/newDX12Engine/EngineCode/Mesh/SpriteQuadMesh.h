#pragma once

#include "Core/Mesh.h"
#include "../Component/Sprite/SpriteComponent.h"

class GSpriteQuadMesh : public GMesh
{
public:
	GSpriteQuadMesh();

	void CreateMesh(float InHeight = 1.f, float InWidth = 1.f);

	bool LoadAtlas(const string& InAtlasName, const string& InMetadataFilename, const string& InTextureKey = string());
	bool SetFrame(const string& InFrameName);

	CSpriteComponent* GetSpriteComponent();
};
