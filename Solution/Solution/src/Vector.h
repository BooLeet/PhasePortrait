#pragma once
#include <vector>

template<typename T>
class VectorWrap
{
public:
	std::vector<T> rawData;

	VectorWrap(size_t size = 0,double fillVal = 0) : rawData(std::vector<T>(size, fillVal)){}

	VectorWrap(const std::vector<T>& data) { rawData = data; }

	VectorWrap operator+(const VectorWrap<T>& rhs) const
	{
		size_t size = max(Size(), rhs.Size());
		VectorWrap result = VectorWrap(size);

		for (size_t i = 0; i < size; ++i)
			result.rawData[i] = (*this)[i] + rhs[i];

		return result;
	}

	VectorWrap operator-(const VectorWrap<T>& rhs) const
	{
		size_t size = max(Size(), rhs.Size());
		VectorWrap result = VectorWrap(size);

		for (size_t i = 0; i < size; ++i)
			result.rawData[i] = (*this)[i] - rhs[i];

		return result;
	}

	VectorWrap operator*(T rhs) const
	{
		VectorWrap result = VectorWrap(rawData.size());
		for (size_t i = 0; i < rawData.size(); ++i)
			result.rawData[i] = rawData[i] * rhs;

		return result;
	}

	friend VectorWrap operator*(double lhs, const VectorWrap& rhs)
	{
		return rhs * lhs;
	}

	size_t Size() const
	{
		return rawData.size();
	}

	T operator[](size_t index)
	{
		return index >= Size() ? 0 : rawData[index];
	}

	const T& operator[](size_t index) const
	{
		return index >= Size() ? 0 : rawData[index];
	}
};