
enum EMaterialType
{
	Lambert = 0,  // 粗糙表面
	HalfLambert = 1, // 粗糙表面
	Phong = 2,      // 金属
	BinnPhong = 3,  // 金属
	Wrap = 4,		// 皮肤材质
	Minnaert = 5,   // 凹凸、纤维材质
	Banded = 6,   // 卡通材质
	GradualBanded = 7,   // 渐变卡通材质
	FinalBanded = 8,   // 融合卡通材质
	Back = 9,
	AnisotropyKajiyaKay= 10,	//各向异性 Kajiya-Kay Shading Model（卡吉雅模型）
	OrenNayar=11,				//GDC 模拟粗糙表面
	Fresnel=100, //测试用，并非单独材质
	Max,
};