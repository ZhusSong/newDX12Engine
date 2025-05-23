﻿#include "CoreMinimalObject.h"

// 全局对象池
vector<CCoreMinimalObject*> GObjects;


CCoreMinimalObject::CCoreMinimalObject()
{
	bTick = true;
	GObjects.push_back(this);
}

CCoreMinimalObject::~CCoreMinimalObject()
{
	for (vector<CCoreMinimalObject*>::const_iterator Iter = GObjects.begin();
		Iter != GObjects.end();
		++Iter)
	{
		if (*Iter == this)
		{
			GObjects.erase(Iter);
			break;
		}
	}
}
