#pragma once


// 代码反射宏
// コードリフレクションマクロ定義
#define CODE_REFLECTOR_MACRO_INNER(A,B,C,D) A##B##C##D
#define CODE_REFLECTOR_MACRO(A,B,C,D) CODE_REFLECTOR_MACRO_INNER(A,B,C,D)

#define CVARIABLE(...) 
#define CDIAPER(...) 
#define CODEREFLECTION(...) \
CODE_REFLECTOR_MACRO(CURRENT_FILE_ID_BT,_,CodeReflectionTagLine,_GENERATED_BODY_BT)
