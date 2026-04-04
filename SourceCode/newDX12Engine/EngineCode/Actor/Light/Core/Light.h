#pragma once
#include "../../Core/ActorObject.h"
#include "Light.CodeReflection.h"

class CLightComponent;
//G -> Game
class GLight :public GActorObject
{
	CODEREFLECTION()

	CVARIABLE()
		CLightComponent* LightComponent;
public:
	GLight();

	virtual void Tick(float DeltaTime);
public:
	virtual void SetPosition(const XMFLOAT3& InNewPosition);
	virtual void SetRotation(const fvector_3d& InRotation);
	virtual void SetScale(const fvector_3d& InNewScale);
public:
	void SetLightIntensity(const fvector_3d& InNewLightIntensity);

protected:
	virtual void SetLightComponent(CLightComponent* InNewComponent);

public:
	const CLightComponent* GetLightComponent() const { return  LightComponent; }
};