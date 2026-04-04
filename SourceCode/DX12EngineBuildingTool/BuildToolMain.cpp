#include "DX12EngineBuildingTool.h"
#include "CodeReflection/CollectClassInfoFun/CollectClassInfo.h"
#include "CodeReflection/IntermediateFile/SpawnIntermediateFile.h"
#include <Windows.h>

//注:需为BuildingTool解决方案在连接器->系统中分配100000000内存
//注: BuildingToolソリューションのリンカー->システムで100000000のメモリを割り当てる必要があります
int main()
{
	char ModulePath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, ModulePath, MAX_PATH);

	char ModuleDirectory[MAX_PATH] = { 0 };
	get_path_directory(ModuleDirectory, ModulePath);
	SetCurrentDirectoryA(ModuleDirectory);

	//if (0)
	//{
	//	char Path[] = "J:\\newDX12Project\\MyEngineBuildingTool\\TestObject.h";
	//	char PathH[] = "J:\\newDX12Project\\MyEngineBuildingTool\\TestObject.G.h";
	//	char PathCPP[] = "J:\\newDX12Project\\MyEngineBuildingTool\\TestObject.G.cpp";
	//	normalization_path(Path);
	//	FClassAnalysis ClassAnalysis;
	//	CollectClassInfo::Collection(Path, ClassAnalysis);

	//	std::vector<std::string> OutAnalysisRawH;
	//	std::vector<std::string> OutAnalysisRawCPP;
	//	IntermediateFile::Builder(ClassAnalysis, OutAnalysisRawH, OutAnalysisRawCPP);

	//	simple_cpp_helper_file::save_file_to_strings(PathH, OutAnalysisRawH);
	//	simple_cpp_helper_file::save_file_to_strings(PathCPP, OutAnalysisRawCPP);
	//}
	//else
	//{
		// 生成反射代码的位置
		// リフレクションコードを生成する場所
		string CodeReflectionPath = FEnginePathHelper::RelativeToAbsolutePath(FEnginePathHelper::GetEngineCodeReflectionPath());
		// 要遍历的源文件位置
		// 走査するソースファイルの場所
		string SourcePath = FEnginePathHelper::RelativeToAbsolutePath(FEnginePathHelper::GetEngineSourcePath());

		// 先移除所有反射代码
		// まずすべてのリフレクションコードを削除
		remove_dir_all_files(CodeReflectionPath.c_str());

		def_c_paths Paths;
		init_def_c_paths(&Paths);

		find_files(SourcePath.c_str(), &Paths, true);

		map<string, vector<string>> Modulars;
		for (int i = 0; i < Paths.index; i++)
		{
			if (find_string(Paths.paths[i], ".vcxproj", 0) != -1)
			{
				normalization_path(Paths.paths[i]);

				char Buff[1024] = { 0 };
				get_path_directory(Buff, Paths.paths[i]);

				Modulars.insert(make_pair(Buff, vector<string>()));
			}
		}

		for (int i = 0; i < Paths.index; i++)
		{
			if (find_string(Paths.paths[i], ".h", 0) != -1)
			{
				// 单位化路径
				// パスを正規化
				normalization_path(Paths.paths[i]);

				std::vector<std::string> StringArray;
				simple_cpp_helper_file::load_file_to_strings(Paths.paths[i], StringArray);

				if (IsCheckAllowCodeReflection(StringArray))
				{
					// 收集类型
					// タイプを収集
					FClassAnalysis ClassAnalysis;
					vector<string>* PathReflectionCodeCpp = NULL;
					for (auto& Tmp : Modulars)
					{
						
						if (find_string(Paths.paths[i], Tmp.first.c_str(), 0) != -1)
						{
							ClassAnalysis.ModularPath = Tmp.first;
							PathReflectionCodeCpp = &Tmp.second;
							break;
						}
					}

					CollectClassInfo::Collection(Paths.paths[i], ClassAnalysis);

					// 构建.h和.cpp代码
					// .hと.cppのコードを構築
					std::vector<std::string> OutAnalysisRawH;
					std::vector<std::string> OutAnalysisRawCPP;
					IntermediateFile::Builder(ClassAnalysis, OutAnalysisRawH, OutAnalysisRawCPP);

					char Buff[1024] = { 0 };
					get_path_clean_filename(Buff, Paths.paths[i]);

					remove_char_end(Buff, 'h');
					remove_char_end(Buff, '.');

					string PathH = CodeReflectionPath + "/" + Buff + ".CodeReflection.h";
					string PathCPP = CodeReflectionPath + "/" + Buff + ".CodeReflection.cpp";

					simple_cpp_helper_file::save_file_to_strings(PathH, OutAnalysisRawH);
					simple_cpp_helper_file::save_file_to_strings(PathCPP, OutAnalysisRawCPP);
				
					if (PathReflectionCodeCpp)
					{
						// 絶対パス
						if (1)
						{
							PathReflectionCodeCpp->push_back(
								simple_cpp_string_algorithm::printf(
									"#include \"%s\"", PathCPP.c_str()));
						}
						else // 相対パス
						{
							PathReflectionCodeCpp->push_back(
								simple_cpp_string_algorithm::printf(
									"#include \"%s.CodeReflection.cpp\"", Buff));
						}
					}
				}
			}
		}
	
		//　リング
		for (auto& Tmp : Modulars)
		{
			if (Tmp.second.size() > 0)
			{
				string LinkCodeReflection = simple_cpp_string_algorithm::printf(
					"%sLinkCodeReflection.build.cpp", Tmp.first.c_str());

				simple_cpp_helper_file::save_file_to_strings(LinkCodeReflection, Tmp.second);
			}
		}
	//}
	return 0;
}
