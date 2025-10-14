#pragma once

#ifndef RI_ENGINECOREOBJECT_EXPORTS
#define RI_ENGINECOREOBJECT_EXPORTS
#endif

#ifdef RI_ENGINECOREOBJECT_EXPORTS
#define RI_ENGINECOREOBJECT_API __declspec(dllexport)
#else
#define RENZHAICOREOBJECT_API __declspec(dllimport)
#endif // EEE