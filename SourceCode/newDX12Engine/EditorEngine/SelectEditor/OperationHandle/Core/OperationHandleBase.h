#pragma once
#include "../../../../EngineCode/Actor/Core/ActorObject.h"
#include "../../../../EngineCode/Interface/DirectXDeviceInterface.h"
#include "../../../../EngineCode/Component/Input/InputType.h"

#include "OperationHandleBase.CodeReflection.h"
class CCustomMeshComponent;
class CInputComponent;
class CComponent;

enum EMeshRenderLayerType;

// 移动、缩放与旋转手柄基类
// 移動、拡大縮小、回転ハンドルの基底クラス
class GOperationHandleBase:public GActorObject, public IDirectXDeviceInterface
{
	CODEREFLECTION()
protected:
	enum ESelectAxisType
	{
		SELECTAXIS_NONE,
		SELECTAXIS_X,//x
		SELECTAXIS_Y,//y
		SELECTAXIS_Z,//z
		SELECTAXIS_ANY,//A
	};


protected:
	CVARIABLE()
		CInputComponent* InputComponent;

	CVARIABLE()
		CCustomMeshComponent* XAxisComponent;

	CVARIABLE()
		CCustomMeshComponent* YAxisComponent;

	CVARIABLE()
		CCustomMeshComponent* ZAxisComponent;

	CVARIABLE()
		CCustomMeshComponent* AxisComponent;

public:
	GOperationHandleBase();

	virtual void SetMeshRenderLayerType(EMeshRenderLayerType InRenderLayerType);

public:
	virtual void SetPosition(const XMFLOAT3& InNewPosition);
	virtual void SetScale(const fvector_3d& InNewScale);

public:
	ESelectAxisType GetSelectAxis();
	void ResetVisible(CCustomMeshComponent* InAxisComponent, bool bVisible);
	void ResetVisible();
	void ResetColor();
	void ResetColor(CCustomMeshComponent* InAxisComponent, const fvector_4d& InColor);

	// 是否点击到物体
	// オブジェクトがクリックされたかどうか
	void SetHitActor(bool isHit) { bIsHitActor = isHit; };

public:
	virtual void BeginInit();
	virtual void Tick(float DeltaTime);
	virtual void SetVisible(bool bNewVisible);
	// virtual bool IsVisible()const;


protected:
	virtual fvector_3d AnyAxis(
		const fvector_3d& InRayWorldOriginPoint,
		const fvector_3d& InRayWorldDirection,
		const fvector_3d& InActorWorldOriginPoint);

	virtual fvector_3d GetSelectedObjectDirection(
		const fvector_3d& WorldOriginPoint,
		const fvector_3d& WorldDirection,
		const fvector_3d& ActorWorldPosition);

protected:
	virtual void OnMouseMove(int X, int Y);

	virtual void OnLeftMouseButtonDown(int X, int Y);
	virtual void OnLeftMouseButtonUp(int X, int Y);

	virtual void OnRMouseButtonDown(int X, int Y);//Right
	virtual void OnRMouseButtonUp(int X, int Y);//Right

	virtual void OnCaptureKeyboardInfor(const FInputKey& InKey);

protected:
	// 通过屏幕映射计算物体企图移动的下一个位置
	// スクリーン座標のマッピングにより、オブジェクトが移動しようとする次の位置を計算する
	float GetMouseCreenMovePosition(int X, int Y,
		fvector_3d& ActorWorldPosition,
		fvector_3d& ActorWorldDir);

	// 判断当前选择的手柄
	// 現在選択されているハンドルかどうかを判定する
	bool IsCurrentSelectedHandle();
protected:
	// 缩放标准值
	// スケーリングの基準値
	float FixedZoom;

	std::vector<CComponent*> IgnoreComponents;
	bool bPressRightMouse;

	// 未选中目标时操作手柄的位置
	// 対象が未選択時の操作ハンドルの位置
	XMFLOAT3 HiddenPosition = XMFLOAT3(10000, 10000, 10000);

	// 是否点击到物体
	// オブジェクトがクリックされたかどうか
	bool bIsHitActor = false;
};