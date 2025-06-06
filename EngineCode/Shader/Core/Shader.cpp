#include "Shader.h"

LPVOID FShader::GetBufferPointer() const
{
	return ShaderCode->GetBufferPointer();
}

SIZE_T FShader::GetBufferSize() const
{
	return ShaderCode->GetBufferSize();
}


void FShader::BuildShaders(const wstring& InFileName, const string& InEntryFunName, const string& InShadersVersion)
{
	ComPtr<ID3DBlob> ErrorShaderMsg;
	HRESULT R = D3DCompileFromFile(InFileName.c_str(),
		NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		InEntryFunName.c_str(), InShadersVersion.c_str(),
#if _DEBUG
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION
#else
		0
#endif
		, 0, &ShaderCode, &ErrorShaderMsg);

	if (ErrorShaderMsg)
	{
		Engine_Log_Error("%s", (char*)ErrorShaderMsg->GetBufferPointer());
	}

	//失败就奔溃了
	ANALYSIS_HRESULT(R);
}

void FShader::BuildShaderByName(const wstring& InFileName, const string& InEntryFunName, const string& InShadersVersion)
{
	ComPtr<ID3DBlob> ErrorShaderMsg;

	// wstring
	size_t len = InFileName.size();
	wchar_t* shader_path = new wchar_t[len + 1];  // +1 结尾 \0
	std::wmemcpy(shader_path, InFileName.c_str(), len);
	shader_path[len] = L'\0';

	wchar_t* shaderPath = get_shader_path_shader_name(shader_path);

	HRESULT R = D3DCompileFromFile(shaderPath,
		NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		InEntryFunName.c_str(), InShadersVersion.c_str(),
#if _DEBUG
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION
#else
		0
#endif
		, 0, &ShaderCode, &ErrorShaderMsg);

	if (ErrorShaderMsg)
	{
		Engine_Log_Error("%s", (char*)ErrorShaderMsg->GetBufferPointer());
	}
	ErrorShaderMsg.Reset();
	//失败就奔溃了
	ANALYSIS_HRESULT(R);

	

}
