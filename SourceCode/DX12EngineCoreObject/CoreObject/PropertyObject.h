#pragma once
#include "FieldObject.h"

class RI_ENGINECOREOBJECT_API CPropertyObject :public CFieldObject
{
public:
	int GetSize() const { return 0; }

	FORCEINLINE void InitializeValue(void* Dest) const {}
};