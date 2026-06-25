#pragma once
enum EPipelineState
{
	Transparent = 0,        //透明　　
	AlphaTest,				//alpha测试　　　　　　　// アルファテスト
	Background,             //背景
	Reflector,              //反射
	GrayModel = 4,			//实体　　　　　　　　　　// 実体　　　　
	Wireframe = 5,			//线框　　　　　　　　　　// ワイヤーフレーム
	OrthogonalShadow = 6,   //正交阴影(平行光)　　　　// 正射影シャドウ（平行光）
	PerspectiveShadow = 7,  //透视阴影(聚光灯)　　　　// 透視シャドウ（スポットライト）
	OmnidirectionalShadow = 8,  //万向阴影(点光)　　　// 全方向シャドウ（点光源）
	Select=9,                //选择描边　　　　　　　// 選択アウトライン
	Operation_Handle = 10,    //方向箭头　　　　　　　// 操作用ハンドル
	Operation_Handle_Rot_Plane = 11, //旋转面片　　　// 回転ハンドル
	State_Normal = 20,         // 法线　　　　　　　　// 法線
	State_SSAO = 21,           // SSAO
	State_SSAO_Bilateral = 22, // SSAO双边模糊
	PlanarOpaque = 23,         // 平面反射-不透明       // 平面反射-不透明
	PlanarTransparent = 24,    // 平面反射-透明         // 平面反射-透明
	PlanarReflector = 25,      // 平面反射-反射体       // 平面反射-反射体
};

// 渲染条件
// レンダリング条件
enum ERenderingConditions
{
	RC_None = 0,//全部渲染　　    // 全て描画
	RC_Shadow,//仅渲染该物体　　　 // このオブジェクトのみ描画
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
