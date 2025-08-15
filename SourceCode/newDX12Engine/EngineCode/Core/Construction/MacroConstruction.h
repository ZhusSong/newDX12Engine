#pragma once
//25.6.11 李
// Actor对象Mesh数据相关宏
#include "ActorMeshConstruction.h"

// 创建Mesh渲染信息宏
#define CREATE_RENDER_DATA(T,...)\
ActorMeshConstruction::CreateMeshRenderData<T>(GetMeshManager(),this,__VA_ARGS__);

// 通过组件创建Mesh渲染信息宏
#define CREATE_RENDER_DATA_BY_COMPONENT(T,Component,...)\
ActorMeshConstruction::CreateMeshRenderDataByComponent<T>(GetMeshManager(), dynamic_cast<T*>(Component) , __VA_ARGS__);

//##args