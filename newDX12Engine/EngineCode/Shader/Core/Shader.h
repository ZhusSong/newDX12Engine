#pragma once
// 2025.4.李
#include "../../EngineMinimal.h"

class FShader
{
public:
	LPVOID GetBufferPointer() const; 
	SIZE_T GetBufferSize() const;

	// 通过指定路径构建shader
	void BuildShaders(
		const wstring& InFileName,
		const string& InEntryFunName,
		const string& InShadersVersion,
		const D3D_SHADER_MACRO* InShaderMacro = NULL);

	void BuildShaderByName(
		const wstring& InFileName,
		const string& InEntryFunName,
		const string& InShadersVersion,
		const D3D_SHADER_MACRO* InShaderMacro = NULL);
private:
	ComPtr<ID3DBlob> ShaderCode;
	// 通过shader名字获取相对路径并构建shader
	
};