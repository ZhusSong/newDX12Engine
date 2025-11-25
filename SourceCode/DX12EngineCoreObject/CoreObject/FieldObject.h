#pragma once
#include "CoreMinimalObject.h"

// 提供核心字段对象
class RI_ENGINECOREOBJECT_API CFieldObject :public CCoreMinimalObject
{
	typedef CCoreMinimalObject Super;

public:
	CFieldObject();

	CFieldObject* Nest;
};