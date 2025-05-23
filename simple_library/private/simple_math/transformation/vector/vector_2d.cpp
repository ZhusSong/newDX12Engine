﻿// 2025.4 李
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "simple_library/public/simple_math/transformation/vector/vector_2d.h"
#include <stdio.h>
#include <math.h>

fvector_2d::fvector_2d()
	:x(0.f)
	,y(0.f)
{
}

fvector_2d::fvector_2d(float in_x, float in_y)
	:x(in_x)
	,y(in_y)
{

}



fvector_2d::fvector_2d(float in_value)
	:x(in_value)
	,y(in_value)
{
}

float fvector_2d::len()
{
	return sqrtf(x * x + y * y);
}

float fvector_2d::dot(const fvector_2d& a, const fvector_2d& b)
{
	return b.x * a.x + b.y * a.y;
}

float fvector_2d::cross_product(const fvector_2d& a, const fvector_2d& b)
{
	return a.x * b.y - a.y * b.x;
}

char *fvector_2d::to_string(char* buff)const
{
	sprintf(buff,"(x=%f,y=%f)",x,y);
	return buff;
}

void fvector_2d::normalize()
{
	*this /= len();
}
