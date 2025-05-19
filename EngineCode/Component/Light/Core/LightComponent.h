// 灯光组件
// 25.5.19 李
#pragma once
#include "../../TransformComponent.h"
#include "../../../Interface/DirectXDeviceInterface.h"

class CMeshComponent;
class CLightComponent :public CTransformComponent, public IDirectXDeviceInterface
{
	typedef CTransformComponent Super;

	CVARIABLE()
		CMeshComponent* LightMesh;
public:
	CLightComponent();

	virtual ~CLightComponent();

	CMeshComponent* GetLightMesh() { return LightMesh; }

	virtual void SetPosition(const XMFLOAT3& InNewPosition);
	virtual void SetRotation(const fvector_3d& InNewRotation);
	virtual void SetScale(const fvector_3d& InNewScale);

	virtual void SetForwardVector(const XMFLOAT3& InForwardVector);
	virtual void SetRightVector(const XMFLOAT3& InRightVector);
	virtual void SetUPVector(const XMFLOAT3& InUPVector);

protected:
	void SetLightMesh(CMeshComponent* InLightMesh);
};
