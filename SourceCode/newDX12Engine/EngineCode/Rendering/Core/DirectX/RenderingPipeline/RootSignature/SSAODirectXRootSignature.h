#pragma once
#include "Core/DirectXRootSignature.h"
#include "../StaticSampler/StaticSamplerObject.h"

// SSAO根签名
struct FSSAODirectXRootSignature :public FDirectXRootSignature
{
private:
	typedef FDirectXRootSignature Super;

public:
	FSSAODirectXRootSignature();

	virtual void BuildRootSignature(UINT InTextureNum = 1);

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);
};