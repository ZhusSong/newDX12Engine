#pragma once
// 2025.4.李
#include "../../EngineMinimal.h"

class FShader
{
public:
	LPVOID GetBufferPointer() const; 
	SIZE_T GetBufferSize() const;

	void BuildShaders(
		const wstring& InFileName,
		const string& InEntryFunName,
		const string& InShadersVersion,
		const D3D_SHADER_MACRO* InShaderMacro = NULL);
private:
	ComPtr<ID3DBlob> ShaderCode;
	void BuildShaderByName(const wstring& InFileName, const string& InEntryFunName, const string& InShadersVersion);
};