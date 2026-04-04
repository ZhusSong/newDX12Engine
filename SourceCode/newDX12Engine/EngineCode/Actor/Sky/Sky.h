#pragma once
#pragma once
#include "../../Mesh/SphereMesh.h"
#include "Sky.CodeReflection.h"

class CSkyComponent;

class GSky :public GSphereMesh
{
	CODEREFLECTION()

public:
	virtual void Tick(float DeltaTime);

public:
	GSky();
};