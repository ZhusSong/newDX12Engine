#pragma once
#include "FieldObject.h"

class CPropertyObject;

class RI_ENGINECOREOBJECT_API CFunctionObject :public CFieldObject
{
public:
	CPropertyObject* Property;
	vector<unsigned char> Script;
};