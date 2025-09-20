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
	OrthogonalShadow = 6,   //正交阴影(平行光)
	PerspectiveShadow = 7,  //透视阴影(聚光灯)
	OmnidirectionalShadow = 8,  //万向阴影(点光)
	Select=9,                //选择描边
	Operation_Handle = 10,    //方向箭头
	Operation_Handle_Rot_Plane = 11, //旋转面片
	State_Normal = 20,         // 法线
	State_SSAO = 21,           // SSAO
	State_SSAO_Bilateral = 22, // SSAO双边模糊
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