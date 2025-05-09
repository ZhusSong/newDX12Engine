#pragma once
#include "../../../../../Interface/DirectXDeviceInterfece.h"

// 根签名结构体
struct FDirectXRootSignature :public IDirectXDeviceInterface_Struct
{
	FDirectXRootSignature();

	void BuildRootSignature();

	void PreDraw(float DeltaTime);
	void Draw(float DeltaTime);
	void PostDraw(float DeltaTime);

	ID3D12RootSignature* GetRootSignature() { return RootSignature.Get(); }
private:
	ComPtr<ID3D12RootSignature> RootSignature;
};