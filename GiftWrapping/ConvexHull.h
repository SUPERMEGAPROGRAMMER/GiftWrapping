#pragma once

#include "stdafx.h"
#include "MathVector.h"

class Face
{
public:
	Face()
	{
		m_data = std::set<size_t>();
	}

	Face(const std::set<size_t> &face)
	{
		m_data = face;
	}

	const bool operator < (const Face &another) const
	{
		// unordered структуры дополнительно изменяются (сортируются)
		//		std::sort(m_data.begin(), m_data.end());
		//		std::sort(another.m_data.begin(), another.m_data.end());

		return std::lexicographical_compare(m_data.begin(),
			m_data.end(),
			another.m_data.begin(),
			another.m_data.end());
	}

	const bool operator == (const Face &another) const
	{
		if (m_data.size() != another.m_data.size())
			return false;

		auto it_1 = m_data.begin();
		auto it_2 = another.m_data.begin();

		while (it_1 != m_data.end())
		{
			if (*it_1 != *it_2)
				return false;

			++it_1;
			++it_2;
		}

		return true;
	}

	friend std::ostream& operator<<(std::ostream &out_stream, const Face &hyperface)
	{
		for (auto index : hyperface.m_data)
			out_stream << index << " ";

		return out_stream;
	}

public:
	std::set<size_t> m_data;
};

size_t us_hash(const Face& S);

class Faces
{
public:
	Faces()
	{
		m_data = std::unordered_set<Face, decltype(&us_hash)>(0, us_hash);
	}

public:
	std::unordered_set<Face, decltype(&us_hash)> m_data;
};

struct ConvexHull
{
	std::vector<MathVector> points;
	Faces faces;
};