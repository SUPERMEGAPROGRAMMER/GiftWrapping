#pragma once

#include "MathVector.h"

size_t us_hash(const std::unordered_set<size_t>& S);

class GiftWrapping
{
public:
	GiftWrapping();
	~GiftWrapping();

	// -----
	void run_algorithm();
	
	std::vector<MathVector> input_points;
	std::vector<MathVector> scatter_points;
	// -----

	std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> convex_hull;

private:
	// -----
	void wrapping_algorithm();

	void scatter_operation();
	// -----

	void find_subfaces(const std::unordered_set<size_t>& hyperface, std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)>& subfaces);
	size_t wrapping(bool is_first_hyperface, const MathVector& normal_of_hyperface, const MathVector& point_of_subface, const MathVector& normal_of_subface, const std::unordered_set<size_t>& indexes_of_candidates);
	void create_coordinate_axis(size_t num_of_coordinate, size_t dimension, MathVector& coordinate_axis);
	void find_first_hyperface(std::unordered_set<size_t>& first_hyperface);

	// -----
	bool b_FinishAlgorithm;// = false;
	bool b_NonSimplicity;// = false;
	// -----
};