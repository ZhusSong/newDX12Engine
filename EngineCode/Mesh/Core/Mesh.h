#pragma once
#include "../../Rendering/Core/Rendering.h"
#include "MeshType.h"
#include "../../Actor/Core/ActorObject.h"
#include "../../Interface/DirectXDeviceInterface.h"

class CTransformComponent;
class CMeshComponent;
class CMaterial;

// 游戏对象网格接口
class GMesh :
	public GActorObject, 
	public IRenderingInterface,
	public IDirectXDeviceInterface
{
	typedef GActorObject Super;

	CVARIABLE()
		CTransformComponent* TransformComponent;

	CVARIABLE()
		CMeshComponent* MeshComponent;
public:
	GMesh();

	virtual void Init();


	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);

public:
	virtual void SetPosition(const XMFLOAT3& InNewPosition);
	virtual void SetRotation(const fvector_3d& InRotation);
	virtual void SetScale(const fvector_3d& InNewScale);
public:
	virtual CMeshComponent* GetMeshComponent() { return MeshComponent; }
protected:
	virtual void SetMeshComponent(CMeshComponent* InMeshComponent);
public:
	UINT GetMaterialNum()const;

	vector<CMaterial*>* GetMaterials();
};