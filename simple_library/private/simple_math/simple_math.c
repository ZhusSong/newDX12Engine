﻿// 2025.4 李
#include "simple_library/public/simple_math/simple_math.h"

#ifndef RAND_MAX
#define RAND_MAX 0x7fff
#endif

unsigned int get_uint32_random(unsigned int in_max)
{
	if (in_max > 0)
	{
		return (unsigned int)(rand() / (double)RAND_MAX * (in_max - 0) + 0);
	}

	return 0;
}
