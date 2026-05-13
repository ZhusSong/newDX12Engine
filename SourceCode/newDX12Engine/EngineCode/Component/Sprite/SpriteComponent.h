#pragma once

#include "../Mesh/Core/MeshComponent.h"
#include "../../Interface/DirectXDeviceInterface.h"

#include "../../Rendering/Core/Sprite/SpriteAtlas.h"


class CMaterial;

class CSpriteComponent : public CMeshComponent, public IDirectXDeviceInterface
{
public:
	CSpriteComponent();

	virtual void Init() override;
	virtual void SetScale(const fvector_3d& InNewScale) override;

	void CreateMesh(FMeshRenderingData& MeshData, float InHeight, float InWidth);
	void BuildKey(size_t& OutHashKey, float InHeight, float InWidth);
	void BuildSprite(float InHeight = 1.f, float InWidth = 1.f);

	bool LoadAtlas(const string& InAtlasName, const string& InMetadataFilename, const string& InTextureKey = string());
	bool SetAtlas(const shared_ptr<CSpriteAtlas>& InAtlas);
	bool SetFrame(const string& InFrameName);

	void SetTintColor(const fvector_4d& InTintColor);
	void SetOpacity(float InOpacity);
	void SetPixelsPerUnit(float InPixelsPerUnit);
	void SetAutoSizeToFrame(bool bInAutoSizeToFrame);

	shared_ptr<CSpriteAtlas> GetAtlas() const { return Atlas; }
	const string& GetCurrentFrameName() const { return CurrentFrameName; }
	const FSpriteAtlasFrame* GetCurrentFrame() const;

protected:
	void ApplySpriteState();
	void ApplyMaterialSettings();
	void ApplyTextureTransform();
	void ApplyDisplayScale();
	XMFLOAT4X4 BuildTextureTransformForCurrentFrame() const;

protected:
	shared_ptr<CSpriteAtlas> Atlas;
	string CurrentFrameName;

	fvector_4d TintColor;
	fvector_3d SpriteScale;

	float PixelsPerUnit;
	float Opacity;

	bool bAutoSizeToFrame;
};
