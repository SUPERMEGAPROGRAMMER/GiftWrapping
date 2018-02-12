#pragma once

#include "MathVector.h"
#include "ConvexHull.h"

class GiftWrapping
{
public:
	GiftWrapping(bool _b_random_seed);
	~GiftWrapping();

	void run_algorithm();
	
	std::vector<MathVector> input_points;
	std::vector<MathVector> scatter_points;

	Faces convex_hull;

private:
	void wrapping_algorithm();
	void scatter_operation();

	void find_subfaces(const Face& hyperface, Faces& subfaces);

	size_t wrapping(bool is_first_hyperface, const MathVector& normal_of_hyperface, const MathVector& point_of_subface, const MathVector& normal_of_subface, const std::unordered_set<size_t>& indexes_of_candidates);
	void create_coordinate_axis(size_t num_of_coordinate, size_t dimension, MathVector& coordinate_axis);
	
	void find_first_hyperface(Face& first_hyperface);

	bool b_FinishAlgorithm;
	bool b_NonSimplicity;
	// -----
	bool b_random_seed;
	// -----
};