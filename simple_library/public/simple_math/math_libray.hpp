﻿#pragma once
// 2025.4 李
namespace math_libray
{
	template<typename T>
	T Clamp(const T& InValue, const T& InMin, const T& InMax)
	{
		return InValue < InMin ? InMin : (InValue > InMax ? InMax : InValue);
	}

	template<typename T>
	T Min(const T& InValue, const T& InMin)
	{
		return InValue < InMin ? InValue : InMin;
	}

	template<typename T>
	T Max(const T& InValue, const T& InMax)
	{
		return InValue > InMax ? InValue : InMax;
	}

	template<typename T>
	static T Lerp(const T& InValue, const T& InTarget, float InTime)
	{
		return InValue + (InTarget - InValue) * InTime;
	}
}