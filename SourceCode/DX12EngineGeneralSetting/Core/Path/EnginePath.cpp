#include "EnginePath.h"

std::string FEnginePathHelper::GetEngineRootPath()
{
	return string();
}

std::string FEnginePathHelper::GetEngineBinariesPath()
{
	return "..\\..\\Binaries\\Win64";
}

std::string FEnginePathHelper::GetEngineLogsPath()
{
	return "..\\..\\Saved\\Logs";
}

std::string FEnginePathHelper::GetEngineSourcePath()
{
	return "..\\..\\SourceCode";
}

std::string FEnginePathHelper::GetEngineIntermediatePath()
{
	return "..\\..\\Intermediate";
}

std::string FEnginePathHelper::GetEngineCodeReflectionPath()
{
	return "..\\..\\Intermediate\\CodeReflection";
}

std::string FEnginePathHelper::GetEngineAssetPath()
{
	return "..\\..\\Asset";
}

std::wstring FEnginePathHelper::GetEngineShadersPath()
{
	return L"..\\..\\Shaders";
}

std::string FEnginePathHelper::RelativeToAbsolutePath(const std::string& InPath)
{
	char PathBuff[1024] = { 0 };
	get_full_path(PathBuff, 1024, InPath.c_str());

	return PathBuff;
}
