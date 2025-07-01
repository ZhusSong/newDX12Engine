#pragma once
//25.7.1 李
enum EPipelineState
{
	Transparent = 0,        //透明
	AlphaTest,				//alpha测试
	Background,             //背景
	Reflector,              //反射
	GrayModel = 4,			//实体
	Wireframe = 5,			//线框
	Shadow = 6,
};

// 渲染条件
enum ERenderingConditions
{
	RC_None = 0,//全部渲染
	RC_Shadow,//仅渲染该物体
};

//struct FTextureNumber
//{
//	FTextureNumber()
//		:Texture2DNum(1)
//		, CubeMapNum(1)
//	{}
//
//	UINT Texture2DNum;
//	UINT CubeMapNum;
//};