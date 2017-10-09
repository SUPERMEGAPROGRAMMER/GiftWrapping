#pragma once

#include "MathVector.h"

// -----

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

public:
	std::set<size_t> m_data;
};

// -----
// size_t us_hash(const std::set<size_t>& S);
size_t us_hash(const Face& S);
// -----


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

// -----

class GiftWrapping
{
public:
	GiftWrapping();
	~GiftWrapping();

	void run_algorithm();
	
	std::vector<MathVector> input_points;
	std::vector<MathVector> scatter_points;

	// -----
	// std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> convex_hull;
	// std::unordered_set<std::set<size_t>, decltype(&us_hash)> convex_hull;
	//std::unordered_set<Face, decltype(&us_hash)> convex_hull;
	Faces convex_hull;
	// -----
private:
	void wrapping_algorithm();
	void scatter_operation();

	// -----
	// void find_subfaces(const std::unordered_set<size_t>& hyperface, std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)>& subfaces);
	// void find_subfaces(const std::set<size_t>& hyperface, std::unordered_set<std::set<size_t>, decltype(&us_hash)>& subfaces);
	// void find_subfaces(const Face& hyperface, std::unordered_set<Face, decltype(&us_hash)>& subfaces);
	void find_subfaces(const Face& hyperface, Faces& subfaces);
	// -----

	size_t wrapping(bool is_first_hyperface, const MathVector& normal_of_hyperface, const MathVector& point_of_subface, const MathVector& normal_of_subface, const std::unordered_set<size_t>& indexes_of_candidates);
	void create_coordinate_axis(size_t num_of_coordinate, size_t dimension, MathVector& coordinate_axis);
	
	// -----
	// void find_first_hyperface(std::unordered_set<size_t>& first_hyperface);
	// void find_first_hyperface(std::set<size_t>& first_hyperface);
	void find_first_hyperface(Face& first_hyperface);
	// -----

	bool b_FinishAlgorithm;
	bool b_NonSimplicity;
};