#include "ShaderType.h"

bool ShaderType::ToD3DShaderMacro(const vector<FShaderMacro>& InShaderMacro, vector<D3D_SHADER_MACRO>& OutD3DMacro)
{
	for (auto& Tmp : InShaderMacro)
	{
		D3D_SHADER_MACRO ShaderMacro =
		{
			Tmp.Name.c_str(),
			Tmp.Definition.c_str(),
		};

		OutD3DMacro.push_back(ShaderMacro);
	}

	if (OutD3DMacro.size() > 0)
	{
		D3D_SHADER_MACRO ShaderMacro =
		{
			NULL,
			NULL,
		};

		OutD3DMacro.push_back(ShaderMacro);
	}

	return OutD3DMacro.size();
}
