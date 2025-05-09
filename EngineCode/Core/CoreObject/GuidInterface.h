﻿#pragma once
#include "../../EngineMinimal.h"

//提供一个对象的GUID
class IGuidInterface
{
public:
	IGuidInterface();

	bool operator==(const IGuidInterface& InOther)
	{
		return guid_equal(&Guid, &InOther.Guid);
	}

	simple_c_guid GetGuid() { return Guid; }
private:
	simple_c_guid Guid;
};