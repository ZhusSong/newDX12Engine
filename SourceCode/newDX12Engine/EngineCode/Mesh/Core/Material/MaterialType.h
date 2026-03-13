
enum EMaterialType
{
	Lambert = 0,		// 粗糙表面    // 粗い表面

	HalfLambert = 1,	// 粗糙表面    // 粗い表面

	Phong = 2,			// 金属        // 金属

	BinnPhong = 3,		// 金属        // 金属

	Wrap = 4,			// 皮肤材质    // スキンマテリアル

	Minnaert = 5,		// 凹凸、纤维材质     // 凹凸・繊維マテリアル

	Banded = 6,			// 卡通材质        // トゥーンマテリアル

	GradualBanded = 7,  // 渐变卡通材质     // グラデーショントゥーンマテリアル

	FinalBanded = 8,	// 融合卡通材质     // 融合トゥーンマテリアル

	Back = 9,

	AnisotropyKajiyaKay = 10,	// 各向异性 Kajiya-Kay Shading Model（卡吉雅模型） // 異方性 Kajiya-Kay 

	OrenNayar = 11,			// GDC 模拟粗糙表面       // GDC 粗い表面のシミュレーション

// 不受光照影响的模型渲染模式
// ライティングの影響を受けないモデル描画モード
BaseColor = 12,			// 本身的颜色     // ベースカラー

Normal = 13,			// 局部法线       // ローカル法線

WorldNormal = 14,		// 世界法线       // ワールド法線

Transparency = 15,		// 透明           // 透明

PBR = 20,				// 基于物理的显示    // 物理ベースレンダリング

Fresnel = 100,			// 测试用           // テスト用

ShadowTexture = 101,	// Shadow Debug            // シャドウデバッグ

Max,
};

enum EMaterialDisplayStatusType
{
	DefaultDisplay,       // 默认状态      // デフォルト状態

	PointDisplay,         // 点状态       // ポイント表示状態

	WireframeDisplay,     // 线框状态       // ワイヤーフレーム表示状態

	TriangleDisplay = 4,  // 三角图元显示      // 三角ポリゴン表示
};