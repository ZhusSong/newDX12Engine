#pragma once
#include "Core/OperationHandleBase.h"
#include "MoveArrow.CodeReflection.h"

class GMoveArrow :public GOperationHandleBase
{
	CODEREFLECTION()

public:
	GMoveArrow();

	void CreateMesh();



protected:
	virtual fvector_3d AnyAxis(
		const fvector_3d& InRayWorldOriginPoint,
		const fvector_3d& InRayWorldDirection,
		const fvector_3d& InActorWorldOriginPoint);

protected:
	virtual void OnMouseMove(int X, int Y);
	virtual void OnLeftMouseButtonDown(int X, int Y);
	virtual void OnLeftMouseButtonUp(int X, int Y);

	virtual void OnCaptureKeyboardInfor(const FInputKey& InKey);


protected:
	fvector_3d RelativePosition;

};