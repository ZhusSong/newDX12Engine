#pragma once

#ifndef RI_ENGINESETTING_EXPORTS
#define RI_ENGINESETTING_EXPORTS
#endif

#ifdef RI_ENGINESETTING_EXPORTS
#define RI_ENGINESETTING_API __declspec(dllexport)
#else
#define RI_ENGINESETTING_API __declspec(dllimport)
#endif 