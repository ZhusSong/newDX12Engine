#include "Camera.h"
#include "../Component/InputComponent.h"
#include "../Component/TransformComponent.h"
#include "CameraType.h"
#include "../Config/EngineRenderConfig.h"
#include "../Library/RaycastSystemLibrary.h"
#include "../Rendering/Core/DirectX/RenderingPipeline/RenderLayer/RenderLayerManager.h"
#include "../Component/Mesh/Core/MeshComponentType.h"
#include "../Math/EngineMath.h"

// 添加选择箭头支持
#if EDITOR_ENGINE
#include "../../Common/OperationHandleSelectManager.h"
#endif // 0

extern CMeshComponent* SelectAxisComponent;
extern GActorObject* SelectedObject;

GCamera::GCamera()
	:Super()
{
	FCreateObjectParam Param;
	Param.Outer = this;
	InputComponent = CreateObject<CInputComponent>(Param, new CInputComponent());

	MouseSensitivity = 0.7f;
	CmeraType = ECmeraType::CameraRoaming;

	Radius = 10.f;
	A = XM_PI;//
	B = XM_PI;

	bRightMouseDown = false;
}
void GCamera::BeginInit()
{
	//初始化我们的投影矩阵
	ViewportInit();

	InputComponent->CaptureKeyboardInforDelegate.Bind(this, &GCamera::ExecuteKeyboard);

	InputComponent->OnLMouseButtonUpDelegate.Bind(this, &GCamera::OnLeftMouseButtoUP);
	InputComponent->OnLMouseButtonDownDelegate.Bind(this, &GCamera::OnLeftMouseButtonDown);
	InputComponent->OnRMouseButtonDownDelegate.Bind(this, &GCamera::OnRightMouseButtonDown);
	InputComponent->OnRMouseButtonUpDelegate.Bind(this, &GCamera::OnRightMouseButtonUp);
	InputComponent->OnMouseMoveDelegate.Bind(this, &GCamera::OnMouseMove);
	InputComponent->OnMouseWheelDelegate.Bind(this, &GCamera::OnMouseWheel);
}
	

void GCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 
	
	CmeraType = ECmeraType::CameraRoaming;

	Timeline.Tick(DeltaTime);
}

void GCamera::ExecuteKeyboard(const FInputKey& InputKey)
{
	if (bLeftMouseDown || bRightMouseDown)
	{
		if (InputKey.KeyName == "W")
		{
			MoveForward(1.f);

			SetDirty(true);
		}
		else if (InputKey.KeyName == "S")
		{
			MoveForward(-1.f);

			SetDirty(true);
		}
		else if (InputKey.KeyName == "A")
		{
			MoveRight(-1.f);

			SetDirty(true);
		}
		else if (InputKey.KeyName == "D")
		{
			MoveRight(1.f);

			SetDirty(true);
		}
	}

	if (InputKey.KeyName == "F")
	{
		if (!bFPress)
		{
			FTimelineDelegate TimelineDelegate;
			TimelineDelegate.Bind(this, &GCamera::LookAtAndMoveToSelectedObject);
			Timeline.BindTimeLineEvent(0.4f, TimelineDelegate);

			bFPress = true;
		}
	}

	// 观察模式
	//if (InputKey.KeyName == "alt")
	//{
	//	CmeraType = ECmeraType::ObservationObject;

	//	SetDirty(true);
	//}
}

void GCamera::BuildViewMatrix(float DeltaTime)
{
	switch (CmeraType)
	{
	case CameraRoaming:
	{
		Super::BuildViewMatrix(DeltaTime);
		break;
	}
	case ObservationObject:
	{
		XMFLOAT3& CameraPos = GetRootComponent()->GetPosition();

		CameraPos.x = Radius * sinf(B) * cosf(A);
		CameraPos.z = Radius * sinf(B) * sinf(A);
		CameraPos.y = Radius * cosf(B);

		XMVECTOR Pos = XMVectorSet(CameraPos.x, CameraPos.y, CameraPos.z, 1.0f);
		XMVECTOR ViewTarget = XMVectorZero();
		XMVECTOR ViewUp = XMVectorSet(0.f, 1.0f, 0.f, 0.f);

		XMMATRIX ViewLookAt = XMMatrixLookAtLH(Pos, ViewTarget, ViewUp);
		XMStoreFloat4x4(&ViewMatrix, ViewLookAt);

		break;
	}
	}
}

void GCamera::OnLeftMouseButtonDown(int X, int Y)
{
	bLeftMouseDown = true;

	LastMousePosition.x = X;
	LastMousePosition.y = Y;

	OnClickedScreen(X, Y);

	SetCapture(GetMainWindowsHandle());
}

void GCamera::OnLeftMouseButtoUP(int X, int Y)
{
	bLeftMouseDown = false;
}

void GCamera::OnRightMouseButtonDown(int X, int Y)
{
	bRightMouseDown = true;

	LastMousePosition.x = X;
	LastMousePosition.y = Y;

	//OnClickedScreen(X, Y);

	SetCapture(GetMainWindowsHandle());

	SetDirty(true);
}

void GCamera::OnRightMouseButtonUp(int X, int Y)
{
	bRightMouseDown = false;

	ReleaseCapture();

	LastMousePosition.x = X;
	LastMousePosition.y = Y;

	SetDirty(true);
}

void GCamera::OnMouseMove(int X, int Y)
{
	if (bRightMouseDown)
	{
		float XRadians = XMConvertToRadians((float)(X - LastMousePosition.x) * MouseSensitivity);
		float YRadians = XMConvertToRadians((float)(Y - LastMousePosition.y) * MouseSensitivity);

		switch (CmeraType)
		{
		case CameraRoaming:
		{
			RotateAroundXAxis(YRadians);
			RotateAroundYAxis(XRadians);
			break;
		}
		case ObservationObject:
		{
			A += (-XRadians);
			B += YRadians;

			A = math_libray::Clamp(A, 0.f, XM_2PI * 2.f);
			break;
		}
		}
	}

	LastMousePosition.x = X;
	LastMousePosition.y = Y;

	SetDirty(true);
	
}

void GCamera::OnMouseWheel(int X, int Y, float InDelta)
{
	if (CmeraType == ECmeraType::ObservationObject)
	{
		Radius += (InDelta / 100.f);

		// 限制在一定的范围内
		Radius = math_libray::Clamp(Radius, 7.f, 40.f);
	}

	SetDirty(true);
}

// 移动
void GCamera::MoveForward(float InValue)
{
	if (CmeraType == ECmeraType::CameraRoaming)
	{
		XMFLOAT3 AT3Position = GetRootComponent()->GetPosition();
		XMFLOAT3 AT3ForwardVector = GetRootComponent()->GetForwardVector();

		XMVECTOR AmountMovement = XMVectorReplicate(InValue * 1.f);
		XMVECTOR Forward = XMLoadFloat3(&AT3ForwardVector);
		XMVECTOR Position = XMLoadFloat3(&AT3Position);

		XMStoreFloat3(&AT3Position, XMVectorMultiplyAdd(AmountMovement, Forward, Position));
		GetRootComponent()->SetPosition(AT3Position);
	}
}

void GCamera::MoveRight(float InValue)
{
	if (CmeraType == ECmeraType::CameraRoaming)
	{
		XMFLOAT3 AT3Position = GetRootComponent()->GetPosition();
		XMFLOAT3 AT3RightVector = GetRootComponent()->GetRightVector();

		XMVECTOR AmountMovement = XMVectorReplicate(InValue * 1.f);
		XMVECTOR Right = XMLoadFloat3(&AT3RightVector);
		XMVECTOR Position = XMLoadFloat3(&AT3Position);

		XMStoreFloat3(&AT3Position, XMVectorMultiplyAdd(AmountMovement, Right, Position));
		GetRootComponent()->SetPosition(AT3Position);
	}
}




void GCamera::OnClickedScreen(int X, int Y)
{
	if (!SelectAxisComponent)
	{
		FCollisionResult CollisionResult;
		FRaycastSystemLibrary::HitResultByScreen(GetWorld(), X, Y, CollisionResult);

		if (CollisionResult.bHit)
		{
			Engine_Log("Clicked successfully.[time]=%f", CollisionResult.Time);

			if (FRenderLayerManager* InLayer = GetRenderLayerManager())
			{
				InLayer->HighlightDisplayObject(CollisionResult.RenderingData);
			}


#if EDITOR_ENGINE
			// 设置选择对象
			FOperationHandleSelectManager::Get()->SetNewSelectedObject(CollisionResult.Actor);

			// 显示操作手柄
			FOperationHandleSelectManager::Get()->DisplaySelectedOperationHandle();

			// 设置是否点击到物体
			FOperationHandleSelectManager::Get()->SetHitObject(true);
#endif
		}
		else
		{
			if (FRenderLayerManager* InLayer = GetRenderLayerManager())
			{
				InLayer->Clear(EMeshRenderLayerType::RENDERLAYER_SELECT);
			}


#if EDITOR_ENGINE
			// 设置选择对象
			FOperationHandleSelectManager::Get()->SetNewSelectedObject(nullptr);

			// 隐藏操作手柄
			FOperationHandleSelectManager::Get()->HideSelectedOperationHandle();
			// 设置是否点击到物体
			FOperationHandleSelectManager::Get()->SetHitObject(false);
#endif
		}
	}
	
	
}



void GCamera::RotateAroundXAxis(float InRotateDegrees)
{
	// 拿到相机的方向
	XMFLOAT3 RightVector = GetRootComponent()->GetRightVector();
	XMFLOAT3 UPVector = GetRootComponent()->GetUPVector();
	XMFLOAT3 ForwardVector = GetRootComponent()->GetForwardVector();

	// 拿到关于Y的旋转矩阵
	XMMATRIX RotationX = XMMatrixRotationAxis(XMLoadFloat3(&GetRootComponent()->GetRightVector()), InRotateDegrees);

	// 计算各个方向和按照Z轴旋转后的最终效果
	XMStoreFloat3(&GetRootComponent()->GetUPVector(), XMVector3TransformNormal(XMLoadFloat3(&UPVector), RotationX));
	XMStoreFloat3(&GetRootComponent()->GetForwardVector(), XMVector3TransformNormal(XMLoadFloat3(&ForwardVector), RotationX));
}

void GCamera::RotateAroundYAxis(float InRotateDegrees)
{
	// 拿到相机的方向
	XMFLOAT3 RightVector = GetRootComponent()->GetRightVector();
	XMFLOAT3 UPVector = GetRootComponent()->GetUPVector();
	XMFLOAT3 ForwardVector = GetRootComponent()->GetForwardVector();

	// 拿到关于Z的旋转矩阵
	XMMATRIX RotationY = XMMatrixRotationY(InRotateDegrees);

	// 计算各个方向和按照Z轴旋转后的最终效果
	XMStoreFloat3(&GetRootComponent()->GetRightVector(), XMVector3TransformNormal(XMLoadFloat3(&RightVector), RotationY));
	XMStoreFloat3(&GetRootComponent()->GetUPVector(), XMVector3TransformNormal(XMLoadFloat3(&UPVector), RotationY));
	XMStoreFloat3(&GetRootComponent()->GetForwardVector(), XMVector3TransformNormal(XMLoadFloat3(&ForwardVector), RotationY));
}

void GCamera::LookAtAndMoveToSelectedObject(float InTime, float InDeltaTime)
{
	if (SelectedObject)
	{
		BoundingBox SelectAABB;
		SelectedObject->GetBoundingBox(SelectAABB);

		fvector_3d Extents = EngineMath::ToVector3d(SelectAABB.Extents);

		// 离选择对象的距离
		float R = Extents.len();
		float H = 5.f;
		float FOV = GetFOV();
		assert(FOV != 0.f);

		float L = (R + H) / tan(FOV / 2.f);

		fvector_3d CameraPosition = EngineMath::ToVector3d(GetPosition());
		fvector_3d SelectedObjectPosition = EngineMath::ToVector3d(SelectedObject->GetPosition());

		fvector_3d CameraForwardVector = SelectedObjectPosition - CameraPosition;
		CameraForwardVector.normalize();

		fvector_3d CameraEndPosition = SelectedObjectPosition + CameraForwardVector * (-1.f) * L;

		fvector_3d CurrentCameraPosition = EngineMath::Lerp(CameraPosition, CameraEndPosition, InDeltaTime / InTime);

		SetPosition(EngineMath::ToFloat3(CurrentCameraPosition));

		// 是否启用四元数
		float LerpSpeed = 4.f / InTime;
		if (true)
		{
			fquat Q1 = GetRotationQuat();
			fquat Q2 = EngineMath::BuildQuat(CameraForwardVector);

			fquat CurrentQ = fquat::lerp(Q1, Q2, InDeltaTime * LerpSpeed);

			SetRotationQuat(CurrentQ);
		}
		// 欧拉角做插值
		else
		{
			frotator Rotator1 = GetRotation();
			frotator Rotator2 = EngineMath::BuildRotatorMatrix(CameraForwardVector);

			frotator CurrentRotator = EngineMath::Lerp(Rotator1, Rotator2, InDeltaTime * LerpSpeed);

			SetRotation(CurrentRotator);

			/*Engine_Log("Rotator1 y=%f,r=%f,p=%f", Rotator1.yaw, Rotator1.roll, Rotator1.pitch);
			Engine_Log("Rotator2 y=%f,r=%f,p=%f", Rotator2.yaw, Rotator2.roll, Rotator2.pitch);*/
		}
	}

	bFPress = false;
}

