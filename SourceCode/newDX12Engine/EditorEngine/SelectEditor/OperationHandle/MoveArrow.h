#pragma once
// 25.8.15 李
#include "Core/OperationHandleBase.h"

class GMoveArrow :public GOperationHandleBase
{
	typedef GOperationHandleBase Super;

public:
	GMoveArrow();

	void CreateMesh();

protected:
	virtual void OnMouseMove(int X, int Y);
	virtual void OnLeftMouseButtonDown(int X, int Y);
	virtual void OnLeftMouseButtonUp(int X, int Y);

	// 通过屏幕映射到物体企图移动的下一个位置
	float GetMouseScreenMovePosition(int X, int Y,
		fvector_3d& ActorWorldPosition,
		fvector_3d& ActorWorldDir);
protected:
	fvector_3d RelativePosition;
};