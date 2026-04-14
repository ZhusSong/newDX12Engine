#pragma once
#include "../../EngineMinimal.h"
#include "CoreObject/CoreMinimalObject.h"
#include "ActorObject.CodeReflection.h"

// 反射测试
// コードリフレクションのテスト
class CTransformComponent;
//G -> Game
class GActorObject :public CCoreMinimalObject
{
	//// 反射测试
	// コードリフレクションのテスト
	CODEREFLECTION()

		CVARIABLE()
		CTransformComponent* RootComponent;

public:
	GActorObject();

public:
	FORCEINLINE CTransformComponent* GetRootComponent() const { return RootComponent; }

	// 获取碰撞盒 
	// コリジョンボックスを取得する
	void GetBoundingBox(BoundingBox& OutBoundingBox);
	BoundingBox GetBoundingBox();
public:
	virtual void SetPosition(const XMFLOAT3& InNewPosition);
	// 在当前基础上进行旋转
	// 現在の位置を基点として回転させる
	virtual void SetRotation(const fvector_3d& InRotation);
	// 设置世界坐标下的绝对旋转
	// 世界座標系における絶対回転の設定
	virtual void SetRotation(const fvector_3d& InRotation, bool bAbsoluteRotation);
	// 设置世界坐标下的绝对旋转
	// 世界座標系における絶対回転の設定
	virtual void SetRotation(const frotator& InNewRotation);
	// 设置四元数旋转
	// クォータニオン回転を設定する
	virtual void SetRotationQuat(const fquat& InNewQuatRotation);
	virtual void SetScale(const fvector_3d& InNewScale);


	virtual void SetPickup(bool bNewPickup);
public:
	XMFLOAT3& GetPosition();

	frotator GetRotation()const;


	fvector_3d GetScale()const;

	// 四元数
	// クォータニオン
	fquat GetRotationQuat() const;

	XMFLOAT3& GetForwardVector();
	XMFLOAT3& GetRightVector();
	XMFLOAT3& GetUPVector();



};
