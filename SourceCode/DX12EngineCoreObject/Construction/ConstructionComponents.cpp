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
		if (InParentComponent != InComponent)
		{
			// 设置到子类
			InParentComponent->AddChildren(InComponent);

			// 设置父类
			InComponent->SetParentComponent(InParentComponent);
		}
	}

	void ConstructionComponents(CCoreMinimalObject* InOuter, CCoreMinimalObject* NewObject)
	{
		if (CComponent* InComponent = dynamic_cast<CComponent*>(NewObject))
		{
			// 判断Outer
			if (InOuter)
			{
				// Outer是一个组件
				if (CComponent* InOuterComponent = dynamic_cast<CComponent*>(InOuter))
				{
					UpdateConstructionComponents(InComponent, InOuterComponent);
				}
				// Outer是一个对象
				//else if (GActorObject* InOuterActor = dynamic_cast<GActorObject*>(InOuter))
				//{
				//	// 先得到对象上的组件
				//	if (InOuterActor->GetRootComponent())
				//	{
				//		UpdateConstructionComponents(InComponent, InOuterActor->GetRootComponent());
				//	}
				//}
			}
		}
	}
}