#pragma once
// 25.7.15 李

#include "../../DX12EngineBuildingTool.h"
#include "../CppSourceCodeType.h"

namespace IntermediateFile
{
	bool Builder(
		const FClassAnalysis& InClassAnalysis,
		std::vector<std::string>& OutAnalysisRawH,
		std::vector<std::string>& OutAnalysisRawCPP);
}