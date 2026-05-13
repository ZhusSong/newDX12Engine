#include "SpriteQuadMesh.h"

#include "../Core/Construction/MacroConstruction.h"
#include "../Component/Sprite/SpriteComponent.h"

GSpriteQuadMesh::GSpriteQuadMesh()
{
	BUILD_OBJECT_PARAMETERS(, this);
	SetMeshComponent(ConstructionObject<CSpriteComponent>(Param));
}

void GSpriteQuadMesh::CreateMesh(float InHeight, float InWidth)
{
	CREATE_RENDER_DATA(CSpriteComponent, InHeight, InWidth);
}

bool GSpriteQuadMesh::LoadAtlas(const string& InAtlasName, const string& InMetadataFilename, const string& InTextureKey)
{
	if (CSpriteComponent* SpriteComponent = GetSpriteComponent())
	{
		return SpriteComponent->LoadAtlas(InAtlasName, InMetadataFilename, InTextureKey);
	}

	return false;
}

bool GSpriteQuadMesh::SetFrame(const string& InFrameName)
{
	if (CSpriteComponent* SpriteComponent = GetSpriteComponent())
	{
		return SpriteComponent->SetFrame(InFrameName);
	}

	return false;
}

CSpriteComponent* GSpriteQuadMesh::GetSpriteComponent()
{
	return GetMeshComponent<CSpriteComponent>();
}
