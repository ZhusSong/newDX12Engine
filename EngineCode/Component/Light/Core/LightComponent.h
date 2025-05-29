// 灯光组件
// 25.5.19 李
#pragma once
#include "../../TransformComponent.h"
#include "../../../Interface/DirectXDeviceInterface.h"
#include "LightType.h"

class CMeshComponent;
class CLightComponent :public CTransformComponent, public IDirectXDeviceInterface
{
	typedef CTransformComponent Super;

	CVARIABLE()
		CMeshComponent* LightMesh;
public:
	CLightComponent();

	virtual ~CLightComponent();

	// 设置灯光强度
	void SetLightIntensity(const fvector_3d& InNewLightIntensity) { LightIntensity - InNewLightIntensity; }
public:
	CMeshComponent* GetLightMesh() { return LightMesh; }
	const fvector_3d& GetLightIntensity() { return LightIntensity; }
	ELightType GetLightType() const { return LightType; }

public:
	virtual void SetPosition(const XMFLOAT3& InNewPosition);
	virtual void SetRotation(const fvector_3d& InNewRotation);
	virtual void SetScale(const fvector_3d& InNewScale);

	virtual void SetForwardVector(const XMFLOAT3& InForwardVector);
	virtual void SetRightVector(const XMFLOAT3& InRightVector);
	virtual void SetUPVector(const XMFLOAT3& InUPVector);

protected:
	void SetLightMesh(CMeshComponent* InLightMesh);


protected:
	fvector_3d LightIntensity;
	ELightType LightType;
};
