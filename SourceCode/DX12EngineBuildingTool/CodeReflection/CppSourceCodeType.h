#pragma once

#include "../DX12EngineBuildingTool.h"


// 参数
// パラメータ
struct FParamElement
{
	string Name; //参数名字         // パラメータ名
	bool bConst;//是否是const       // constかどうか
	bool bPointer;//是否是指针*     // ポインタ*かどうか
	bool bReference;//是否是引用&   // 参照&かどうか
	string Type;//float int ...

	FParamElement()
		: bPointer(false)
		, bReference(false)
		, bConst(false)
	{}
};

// 变量 
// 変数
struct FVariableAnalysis :public FParamElement
{
	string CodeType;
	bool bStatic; //是否是静态  //静的かどうか

	FVariableAnalysis()
		:bStatic(false)
	{}
};

// 函数解析
// 関数解析
struct FFunctionAnalysis
{
	std::vector<FParamElement> ParamArray;//函数的静态值   // 関数の静的値
	FParamElement Return;//返回类型                        // 戻り値の型

	string FunctionName;//函数名字                         // 関数名
	bool bStatic;//静态                                    // 静的
	bool bVirtual;//虚拟                                   // 仮想

	string CodeType;//代码类型                             // コードタイプ

	FFunctionAnalysis()
		:bStatic(false)
		, bVirtual(false)
	{}
};

// 类解析成员
// クラス解析メンバ
struct FClassAnalysis
{
	std::vector<FFunctionAnalysis> Function;//函数      // 関数
	std::vector<FVariableAnalysis> Variable;//多少变量  // 変数数

	string APIName;//_API的名称                         // _APIの名前
	string ClassName;//类名称                           // クラス名

	std::vector<string> InheritName;//继承者名字        // 継承元クラス名

	string CodeCPPName;//CPP名字                        // CPPファイル名
	string Filename;//.h文件路径                        // .hファイルパス
	int CodeLine;//反射宏标记在哪行？                   // リフレクションマクロがマークされている行
	string ModularPath;
};

enum ECollectionParmType
{
	Type_Return,//是返回      // 戻り値
	Type_Parm,//参数          // パラメータ
};

bool IsCheckAllowCodeReflection(const std::vector<std::string>& InContent);