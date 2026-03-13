#pragma once


#include "../DX12EngineCoreObjectMacro.h"
#include "../CoreObject/CoreMinimalObject.h"
#include "ConstructionComponents.h"

class CCoreMinimalObject;
class CComponent;
// 创建对象结构
// オブジェクト構造を作成
struct RI_ENGINECOREOBJECT_API FCreateObjectParam
{
	FCreateObjectParam()
		:Outer(nullptr)
		, ParentComponent(NULL)
	{}

	CCoreMinimalObject* Outer;
	CComponent* ParentComponent;
};


template<class T>
T* CreateObject(const FCreateObjectParam& InObjectParam, CCoreMinimalObject* NewObject)
{
	// 检测是不是组件 是组件则按照组件规则注册
	// コンポーネントかどうかを検出し、コンポーネントであればコンポーネントルールに従って登録
	ConstructionComponent::ConstructionComponents(InObjectParam.Outer, NewObject);

	T* Obj = dynamic_cast<T*>(NewObject);
	Obj->SetOuter(InObjectParam.Outer);

	return Obj;
}

template<class T, typename ...ParamTypes>
T* ConstructionObject(const FCreateObjectParam& InObjectParam, ParamTypes &&...Params)
{
	return CreateObject<T>(InObjectParam, new T(Params...));
}
