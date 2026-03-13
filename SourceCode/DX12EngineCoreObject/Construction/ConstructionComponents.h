
class CCoreMinimalObject;
class CComponent;
// 构造组件的层级
// コンポーネントのツリー構造による階層管理
namespace ConstructionComponent
{
	void  UpdateConstructionComponents(CComponent* InComponent, CComponent* InParentComponent);
	void  ConstructionComponents(CCoreMinimalObject* InOuter, CCoreMinimalObject* NewObject);
}