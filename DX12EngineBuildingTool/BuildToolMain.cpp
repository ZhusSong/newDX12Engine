#include "DX12EngineBuildingTool.h"
#include "CodeReflection/CollectClassInfoFun/CollectClassInfo.h"
#include "CodeReflection/IntermediateFile/SpawnIntermediateFile.h"

int main()
{
	char Path[] = "J:\\newDX12Project\\MyEngineBuildingTool\\TestObject.h";
	char PathH[] = "J:\\newDX12Project\\MyEngineBuildingTool\\TestObject.G.h";
	char PathCPP[] = "J:\\newDX12Project\\MyEngineBuildingTool\\TestObject.G.cpp";
	normalization_path(Path);
	FClassAnalysis ClassAnalysis;
	CollectClassInfo::Collection(Path, ClassAnalysis);

	std::vector<std::string> OutAnalysisRawH;
	std::vector<std::string> OutAnalysisRawCPP;
	IntermediateFile::Builder(ClassAnalysis, OutAnalysisRawH, OutAnalysisRawCPP);

	simple_cpp_helper_file::save_file_to_strings(PathH, OutAnalysisRawH);
	simple_cpp_helper_file::save_file_to_strings(PathCPP, OutAnalysisRawCPP);
	return 0;
}