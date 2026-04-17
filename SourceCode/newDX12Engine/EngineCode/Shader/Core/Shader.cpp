#include "Shader.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

LPVOID FShader::GetBufferPointer() const
{
	return ShaderCode->GetBufferPointer();
}

SIZE_T FShader::GetBufferSize() const
{
	return ShaderCode->GetBufferSize();
}

void FShader::BuildShaders(
	const wstring& InFileName,
	const string& InEntryFunName,
	const string& InShadersVersion,
	const D3D_SHADER_MACRO* InShaderMacro,
	UINT InCompileFlags)
{
	UINT CompileFlags = InCompileFlags;
	if (CompileFlags == UINT(-1))
	{
#if _DEBUG
		CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		CompileFlags = 0;
#endif
	}

	ComPtr<ID3DBlob> ErrorShaderMsg;
	HRESULT R = D3DCompileFromFile(InFileName.c_str(),
		InShaderMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		InEntryFunName.c_str(), InShadersVersion.c_str(),
		CompileFlags
		, 0, &ShaderCode, &ErrorShaderMsg);

	if (ErrorShaderMsg)
	{
		char* p = (char*)ErrorShaderMsg->GetBufferPointer();
		if (FAILED(R))
		{
#if defined(_WIN32)
			OutputDebugStringA("Shader compile message:\n");
			OutputDebugStringA(p);
			OutputDebugStringA("\n");
#endif
			Engine_Log_Error("%s", "Shader compile failed. See debug output for full message.");
		}
	}

	// 失败则奔溃
	// 失敗した場合はクラッシュ
	ANALYSIS_HRESULT(R);
}
//
//void FShader::BuildShaderByName(
//	const wstring& InFileName,
//	const string& InEntryFunName,
//	const string& InShadersVersion,
//	const D3D_SHADER_MACRO* InShaderMacro)
//{
//	ComPtr<ID3DBlob> ErrorShaderMsg;
//
//	// wstring
//	size_t len = InFileName.size();
//	wchar_t* shader_path = new wchar_t[len + 1];  // +1 结尾 \0
//	std::wmemcpy(shader_path, InFileName.c_str(), len);
//	shader_path[len] = L'\0';
//
//	wchar_t* shaderPath = get_shader_path_shader_name(shader_path);
//
//	HRESULT R = D3DCompileFromFile(shaderPath,
//		InShaderMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		InEntryFunName.c_str(), InShadersVersion.c_str(),
//#if _DEBUG
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION
//#else
//		0
//#endif
//		, 0, &ShaderCode, &ErrorShaderMsg);
//
//	if (ErrorShaderMsg)
//	{
//		char* p = (char*)ErrorShaderMsg->GetBufferPointer();
//		Engine_Log_Error("%s 1", (char*)ErrorShaderMsg->GetBufferPointer());
//	}
//	ErrorShaderMsg.Reset();
//	// 失败则奔溃
//	ANALYSIS_HRESULT(R);
//
//	
//
//}
