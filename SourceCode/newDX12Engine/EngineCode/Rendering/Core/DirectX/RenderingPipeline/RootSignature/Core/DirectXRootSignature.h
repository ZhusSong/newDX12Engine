#pragma once
#include "../../../../../../Interface/DirectXDeviceInterface.h"
#include "../../StaticSampler/StaticSamplerObject.h"

struct FDirectXRootSignature :public IDirectXDeviceInterface_Struct
{
	FDirectXRootSignature();

	virtual void BuildRootSignature(UINT InTextureNum = 1);

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);

	ID3D12RootSignature* GetRootSignature() { return RootSignature.Get(); }

protected:
	ComPtr<ID3D12RootSignature> RootSignature;
	FStaticSamplerObject StaticSamplerObject;
};