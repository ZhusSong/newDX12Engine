﻿// 2025.4 李
#include "simple_library/public/simple_core_minimal/simple_c_core/simple_c_array/simple_c_array_string.h"

void init_string(simple_c_string *array_c)
{
	array_c->size = 0;
	array_c->data = 0;
}

void destroy_string(simple_c_string *array_c)
{
	assert(array_c);

	free(array_c->data);
}

void add_string(char const * in_data, simple_c_string *array_c)
{
	assert(array_c);

	int index = array_c->size;
	array_c->size++;
	array_c->data = realloc(array_c->data, array_c->size * sizeof(str_node));

	strcpy(array_c->data[index].data, in_data);
}

void printf_string(simple_c_string *array_c)
{
	assert(array_c);

	for (int i = 0;i < array_c->size;i++)
	{
		printf("%s \r\n", array_c->data[i].data);
	}
}

char *get_string(int in_index, simple_c_string *array_c)
{
	return array_c->data[in_index].data;
}

void dismantling_string(const char *in_data, const char *str_sub, simple_c_string *array_c)
{
	char buf[8196 * 10] = { 0 };
	strcpy(buf,in_data);

	init_string(array_c);
	char *p = strtok(buf, str_sub);
	add_string(p, array_c);

	while (p)
	{
		if ((p = strtok(NULL, str_sub)) != NULL)
		{
			add_string(p, array_c);
		}
	}
}

char * dismantling_string_by_index(int index, const char *split_str, const char *sub_str, char *buf)
{
	simple_c_string c_str_param;
	dismantling_string(split_str, sub_str, &c_str_param);
	char *p = get_string(index, &c_str_param);
	remove_char_form_end(p, '\n');

	strcpy(buf, p);
	destroy_string(&c_str_param);

	return buf;
}

bool is_exist(char const * in_data, simple_c_string *array_c)
{
	for (int i = 0 ;i < array_c->size;i++)
	{
		if (strcmp(array_c->data[i].data, in_data) == 0)
		{
			return true;
		}
	}

	return false;
}

void dismantling_wstring(const wchar_t* in_data, const wchar_t* str_sub, simple_c_wstring* array_c)
{
	wchar_t buf[8196 * 10] = { 0 };
	wcscpy(buf, in_data);

	init_wstring(array_c);

	wchar_t* p = wcstok(buf, str_sub,NULL);
	add_wstring(p, array_c);

	while (p)
	{
		if ((p = wcstok(NULL, str_sub, NULL)) != NULL)
		{
			add_wstring(p, array_c);
		}
	}
}

void init_wstring(simple_c_wstring* array_c)
{
	array_c->size = 0;
	array_c->data = 0;
}

void destroy_wstring(simple_c_wstring* array_c)
{
	assert(array_c);

	free(array_c->data);
}

void add_wstring(wchar_t const* in_data, simple_c_wstring* array_c)
{
	assert(array_c);

	int index = array_c->size;
	array_c->size++;
	array_c->data = realloc(array_c->data, array_c->size * sizeof(wstr_node));

	wcscpy(array_c->data[index].data, in_data);
}

wchar_t* get_wstring(int in_index, simple_c_wstring* array_c)
{
	return array_c->data[in_index].data;
}
