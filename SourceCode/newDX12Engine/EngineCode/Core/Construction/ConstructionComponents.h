
class CCoreMinimalObject;
class CComponent;
// 构造组件
namespace ConstructionComponent
{
	void UpdateConstructionComponents(CComponent* InComponent, CComponent* InParentComponent);
	void ConstructionComponents(CCoreMinimalObject* InOuter, CCoreMinimalObject* NewObject);
}