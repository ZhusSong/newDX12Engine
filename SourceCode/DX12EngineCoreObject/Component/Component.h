#pragma once

#include "../CoreObject/CoreMinimalObject.h"

// 设置根组件关系
// ルートコンポーネントの関係を設定
class RI_ENGINECOREOBJECT_API CComponent :public CCoreMinimalObject
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