// 25.6.8 李
#pragma once
#include "assert.h" 

struct fvector_4id
{
	float x;
	float y;
	float z;
	float w;

public:
	fvector_4id()
		:x(0.0f)
		, y(0.0f)
		, z(0.0f)
		, w(1.0f)
	{

	}

	fvector_4id(int in_value);

	fvector_4id(int a, int b, int c, int d);
};