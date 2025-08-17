#pragma once
// 25.8.15 李
#include "../../../../EngineCode/Actor/Core/ActorObject.h"
#include "../../../../EngineCode/Interface/DirectXDeviceInterface.h"

class CCustomMeshComponent;
class CInputComponent;
enum EMeshRenderLayerType;

class GOperationHandleBase
	:public GActorObject
	, public IDirectXDeviceInterface
{
	typedef GActorObject Super;
protected:
	enum ESelectAxisType
	{
		SELECTAXIS_NONE,
		SELECTAXIS_X,//x
		SELECTAXIS_Y,//y
		SELECTAXIS_Z,//z
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

public:
	virtual void BeginInit();
	virtual void Tick(float DeltaTime);
	virtual void SetVisible(bool bNewVisible);
	// virtual bool IsVisible()const;
protected:
	virtual void OnMouseMove(int X, int Y);
	virtual void OnLeftMouseButtonDown(int X, int Y);
	virtual void OnLeftMouseButtonUp(int X, int Y);

protected:
	// 缩放标准值
	float FixedZoom;

	
};