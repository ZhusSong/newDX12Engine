#include "ConstructionComponents.h"

#include "../CoreObject/CoreMinimalObject.h"
#include "../Component/Component.h"
//#include "Core/ActorObject.h"
//#include "../../Component/TransformComponent.h"

namespace ConstructionComponent
{
	void UpdateConstructionComponents(CComponent* InComponent, CComponent* InParentComponent)
	{	
		// 不能Attach自己
		// 自分をアタッチできない
		if (InParentComponent != InComponent)
		{
			// 设置到子类
			// 子クラスに設定
			InParentComponent->AddChildren(InComponent);

			// 设置父类
			// 親クラスを設定
			InComponent->SetParentComponent(InParentComponent);
		}
	}

	void ConstructionComponents(CCoreMinimalObject* InOuter, CCoreMinimalObject* NewObject)
	{
		if (CComponent* InComponent = dynamic_cast<CComponent*>(NewObject))
		{
			// 判断Outer
			// Outerを判断
			if (InOuter)
			{
				// Outer是一个组件
				// Outerはコンポーネント
				if (CComponent* InOuterComponent = dynamic_cast<CComponent*>(InOuter))
				{
					UpdateConstructionComponents(InComponent, InOuterComponent);
				}
				// Outer是一个对象
				// Outerはオブジェクト
				//else if (GActorObject* InOuterActor = dynamic_cast<GActorObject*>(InOuter))
				//{
				//	if (InOuterActor->GetRootComponent())
				//	{
				//		UpdateConstructionComponents(InComponent, InOuterActor->GetRootComponent());
				//	}
				//}
			}
		}
	}
}