#pragma once
#include "../../EngineMinimal.h"

class FShader
{
public:
	LPVOID GetBufferPointer() const; 
	SIZE_T GetBufferSize() const;

	// 通过指定路径构建shader
	// 指定されたパスからシェーダーを構築
	void BuildShaders(
		const wstring& InFileName,
		const string& InEntryFunName,
		const string& InShadersVersion,
		const D3D_SHADER_MACRO* InShaderMacro = NULL);

	// 通过shader文件名获取shader(弃用)
	// シェーダーファイル名からシェーダーを取得（非推奨）
	//void BuildShaderByName(
	//	const wstring& InFileName,
	//	const string& InEntryFunName,
	//	const string& InShadersVersion,
	//	const D3D_SHADER_MACRO* InShaderMacro = NULL);
private:
	ComPtr<ID3DBlob> ShaderCode;
	
};