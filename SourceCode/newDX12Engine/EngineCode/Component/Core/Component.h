#pragma once

#include "../../Core/CoreObject/CoreMinimalObject.h"

// 设置根组件关系
class CComponent :public CCoreMinimalObject
{
public:
	CComponent();

	void SetParentComponent(CComponent* InParent) { Parent = InParent; }
	void AddChildren(CComponent* InChildren);
public:
	FORCEINLINE std::vector<CComponent*>& GetChildrens() { return Children; }

protected:
	CComponent* Parent;
	std::vector<CComponent*> Children;
};