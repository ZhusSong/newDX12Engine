#pragma once
// Actor对象Mesh数据相关宏
// ActorオブジェクトのMeshデータ関連マクロ
#include "ActorMeshConstruction.h"
#include "../../Component/TransformComponent.h"

// 创建Mesh渲染信息宏
// Meshレンダリング情報を作成するマクロ
#define CREATE_RENDER_DATA(T,...) \
ActorMeshConstruction::CreateMeshRenderData<T>(GetMeshManager(), this, __VA_ARGS__);

// 通过组件创建Mesh渲染信息宏
// コンポーネントを通してMeshレンダリング情報を作成するマクロ
#define CREATE_RENDER_DATA_BY_COMPONENT(T, Component, ...) \
ActorMeshConstruction::CreateMeshRenderDataByComponent<T>(GetMeshManager(), dynamic_cast<T*>(Component), __VA_ARGS__);

// 适配Actor构建
// Actor構築に適合
#define BUILD_OBJECT_PARAMETERS(Name, CoreActor) \
FCreateObjectParam Param##Name; \
Param##Name.Outer = CoreActor; \
Param##Name.ParentComponent = CoreActor->GetRootComponent();

// 适配组件构建
// コンポーネント構築に適合
#define BUILD_OBJECT_PARAMETERS_BY_COMPONENT(Name, CoreComponent) \
FCreateObjectParam Param##Name; \
Param##Name.Outer = CoreComponent; \
Param##Name.ParentComponent = CoreComponent;

// 适配Object直接构建
// Object直接構築に適合
#define BUILD_OBJECT_PARAMETERS_BY_NO_COMPONENT(Name, CoreComponent) \
FCreateObjectParam Param##Name; \
Param##Name.Outer = CoreComponent;

//##args