#pragma once
#include <vector>

template<typename T>
class Vector
{
public:
	std::vector<T> rawData;

	Vector(size_t size = 0,double fillVal = 0) : rawData(std::vector<T>(size, fillVal)){}

	Vector(const std::vector<T>& data) { rawData = data; }

	Vector operator+(const Vector<T>& rhs) const
	{
		size_t size = max(Size(), rhs.Size());
		Vector result = Vector(size);

		for (size_t i = 0; i < size; ++i)
			result.rawData[i] = (*this)[i] + rhs[i];

		return result;
	}

	Vector operator-(const Vector<T>& rhs) const
	{
		size_t size = max(Size(), rhs.Size());
		Vector result = Vector(size);

		for (size_t i = 0; i < size; ++i)
			result.rawData[i] = (*this)[i] - rhs[i];

		return result;
	}

	Vector operator*(T rhs) const
	{
		Vector result = Vector(rawData.size());
		for (size_t i = 0; i < rawData.size(); ++i)
			result.rawData[i] = rawData[i] * rhs;

		return result;
	}

	friend Vector operator*(double lhs, const Vector& rhs)
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