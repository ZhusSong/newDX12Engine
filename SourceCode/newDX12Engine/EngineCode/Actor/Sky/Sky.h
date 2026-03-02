#pragma once
#pragma once
#include "../../Mesh/SphereMesh.h"

class CSkyComponent;

class GSky :public GSphereMesh
{
	typedef GSphereMesh Super;

public:
	virtual void Tick(float DeltaTime);

public:
	GSky();
};