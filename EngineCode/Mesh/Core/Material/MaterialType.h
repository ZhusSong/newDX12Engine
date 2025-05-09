
enum EMaterialType
{
	Lambert = 0,  // 粗糙表面
	HalfLambert, // 粗糙表面
	Phong,      // 金属
	BinnPhong,  // 金属
	Wrap,		// 皮肤材质
	Minnaert,   // 凹凸、纤维材质
	Banded,   // 卡通材质
	GradualBanded,   // 渐变卡通材质
	FinalBanded,   // 融合卡通材质
	Fresnel=100, //测试用，并非单独材质
	Back,
	Max,
};