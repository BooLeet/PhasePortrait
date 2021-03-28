#pragma once

template <typename T>
T Clamp(const T& val, const T& min, const T& max)
{
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}