#include "stdafx.h"
#include "GiftWrapping.h"

size_t us_hash(const std::unordered_set<size_t>& S)
{
	size_t sum = 0;
	for (auto item : S)
		sum += item;
	return sum;
}

GiftWrapping::GiftWrapping()
{
	test_points = std::vector<MathVector>();
	convex_hull = std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)>(0, us_hash);
}


GiftWrapping::~GiftWrapping()
{
}

void GiftWrapping::wrapping_algorithm()
{
	// -----
	convex_hull.clear();
	// -----

	// Initialize remaining indexes of points, which can be considered.
	std::unordered_set<size_t> interest_indexes_of_points;
	for (size_t counter = 0; counter < test_points.size(); ++counter)
		interest_indexes_of_points.insert(counter);

	// Declare queue of tagged hyperfaces and set of bounding subfaces.
	std::queue<std::unordered_set<size_t>> queue_of_hyperfaces;
	std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> bounding_subfaces(0, us_hash);

	// Find first hyperface.
	std::unordered_set<size_t> current_hyperface;

	// !!!!!
	find_first_hyperface(test_points, current_hyperface);
	//current_hyperface = std::unordered_set<size_t>({ 0, 2, 6 });
	// !!!!!

	// Push first hyperface in queue and find its subfaces.
	queue_of_hyperfaces.push(current_hyperface);
	find_subfaces(current_hyperface, bounding_subfaces);

	std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> subfaces_of_current_hyperface(0, us_hash);

	// -----
	std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> subfaces(0, us_hash);
	std::unordered_set<size_t> new_hyperface;
	size_t new_vertex_index;
	// -----	

	std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> intersection_of_subfaces(0, us_hash);
	std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> subfaces_of_new_hyperface(0, us_hash);

	std::vector<MathVector> cross_product_vectors;

	MathVector normal_of_hyperface;
	MathVector normal_of_subface;

	while (!queue_of_hyperfaces.empty())
	{
		// Pop hyperface from queue and find its subfaces.
		current_hyperface = queue_of_hyperfaces.front();
		queue_of_hyperfaces.pop();
		find_subfaces(current_hyperface, subfaces_of_current_hyperface);

		// -----
		// Find intersection of subfaces_of_current_hyperface and bounding_subfaces.
		// ?????
		subfaces.clear();
		// ?????
		for (auto subface : subfaces_of_current_hyperface)
			if (bounding_subfaces.count(subface))
				subfaces.insert(subface);
		// -----

		// Find n.
		cross_product_vectors.clear();

		for (auto it = std::next(current_hyperface.begin(), 1); it != current_hyperface.end(); ++it)
			cross_product_vectors.push_back(test_points[*it] - test_points[*current_hyperface.begin()]);

		normal_of_hyperface = MathVector::crossProduct(cross_product_vectors);
		normal_of_hyperface.normalize();

		// @@@@@
		// Ориентируем нормаль гиперграни в полупространство под aff(F)
		// Это не подходит для вырожденного случая (несимплициального), т.к. там ск. пр-ие может быть == 0
		for (size_t counter = 0; counter < test_points.size(); ++counter)
			if (!current_hyperface.count(counter))
			{
				if ((test_points[counter] - test_points[*current_hyperface.begin()]) * normal_of_hyperface < 0)
					normal_of_hyperface = -1.0 * normal_of_hyperface;
				break;
			}

		// @@@@@


		for (auto subface : subfaces)
		{
			// Find a.
			cross_product_vectors.clear();
			// -----
			cross_product_vectors.push_back(normal_of_hyperface);
			// -----
			for (auto it = std::next(subface.begin(), 1); it != subface.end(); ++it)
				cross_product_vectors.push_back(test_points[*it] - test_points[*subface.begin()]);

			normal_of_subface = MathVector::crossProduct(cross_product_vectors);
			normal_of_subface.normalize();

			// @@@@@
			// (Возможно можно как-то неявно задать ориентацию вектора a)
			// Ориентируем нормаль подграни в противоположном направлении от вершины данной гиперграни вне текущей подграни
			for (auto index : current_hyperface)
				if (!subface.count(index))
				{
					if ((test_points[index] - test_points[*subface.begin()]) * normal_of_subface > 0)
						normal_of_subface = -1.0 * normal_of_subface;
					break;
				}


			// @@@@@


			// !!!!!



			// Find vertex of a new hyperface.
			// ?????
			interest_indexes_of_points.clear();
			for (size_t counter = 0; counter < test_points.size(); ++counter)
				interest_indexes_of_points.insert(counter);

			for (auto vertex : current_hyperface)
				interest_indexes_of_points.erase(vertex);
			// ?????
			new_vertex_index = wrapping(false, normal_of_hyperface, test_points[*subface.begin()], normal_of_subface, interest_indexes_of_points, test_points);

			// Add new hyperface in queue.
			new_hyperface = subface;
			new_hyperface.insert(new_vertex_index);
			queue_of_hyperfaces.push(new_hyperface);
			// -----

			// Recalculate set of bounding subfaces
			// ~~~~~
			// 1) intersection_of_subfaces = Г П subfaces(F')
			// Г = bounding_subfaces; subfaces(F') = subfaces_of_new_hyperface

			find_subfaces(new_hyperface, subfaces_of_new_hyperface);

			// !!!!!
			intersection_of_subfaces.clear();
			for (auto subface_of_new_hyperface : subfaces_of_new_hyperface)
				if (bounding_subfaces.count(subface_of_new_hyperface))
					intersection_of_subfaces.insert(subface_of_new_hyperface);
			// !!!!!

			// 2) Г = Г U F'
			for (auto subface_of_new_hyperface : subfaces_of_new_hyperface)
				bounding_subfaces.insert(subface_of_new_hyperface);

			// 3) Г = Г \ intersection_of_subfaces
			for (auto subface_from_intersection : intersection_of_subfaces)
				bounding_subfaces.erase(subface_from_intersection);
			// ~~~~~


		}

		//convex_hull.insert(new_hyperface);
		convex_hull.insert(current_hyperface);
	}
}

void GiftWrapping::find_subfaces(const std::unordered_set<size_t>& hyperface, std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)>& subfaces)
{
	// -----
	subfaces.clear();
	// -----

	// -----
	// std::vector<size_t> subface;
	std::unordered_set<size_t> subface;
	// -----


	for (size_t indexSkip = 0; indexSkip < hyperface.size(); ++indexSkip)
	{
		subface.clear();

		// -----
		auto it = hyperface.begin();
		// -----

		for (size_t index = 0; index < hyperface.size(); ++index)
		{
			if (index != indexSkip)
				// -----
				// subface.push_back(hyperface[index]);
				subface.insert(*it);
			// -----
			++it;
		}

		// -----
		// subfaces.push_back(subface);
		subfaces.insert(subface);
		// -----
	}
}

// all_points изменяется в несимлициальном случае, потому std::vector<MathVector>& all_points (без const)
size_t GiftWrapping::wrapping(bool is_first_hyperface, const MathVector& normal_of_hyperface, const MathVector& point_of_subface, const MathVector& normal_of_subface, const std::unordered_set<size_t>& indexes_of_candidates, std::vector<MathVector>& all_points)
{
	// ~~~~~
	double eps = 1e-10;
	// ~~~~~

	MathVector vector_in_new_candidate_hyperface = all_points[*indexes_of_candidates.begin()] - point_of_subface;

	// ~~~~~
	// =====
	double dot_v_a = vector_in_new_candidate_hyperface * normal_of_subface;
	// =====
	double abs_dot_v_n = fabs(vector_in_new_candidate_hyperface * normal_of_hyperface);

	if (abs_dot_v_n < eps)
	{
		if (!is_first_hyperface)
			std::cout << "A non-simplicial case is found" << std::endl;
		// =====
		//if (dot_v_a > 0)
		return *indexes_of_candidates.begin();
		//else
		//	std::cout << "wrapping warning: \"ctg\" should be = +inf" << std::endl;
		// =====
	}

	// ~~~~~

	double ctg = -dot_v_a / abs_dot_v_n;
	double min_ctg = ctg;
	size_t index_of_point_with_min_ctg = *indexes_of_candidates.begin();

	for (auto it = std::next(indexes_of_candidates.begin(), 1); it != indexes_of_candidates.end(); ++it)
	{
		vector_in_new_candidate_hyperface = all_points[*it] - point_of_subface;

		// ~~~~~

		// =====
		dot_v_a = vector_in_new_candidate_hyperface * normal_of_subface;
		// =====

		abs_dot_v_n = fabs(vector_in_new_candidate_hyperface * normal_of_hyperface);

		if (abs_dot_v_n < eps)
		{
			if (!is_first_hyperface)
			{
				std::cout << "A non-simplicial case is found" << std::endl;

				// To generate a random offset vector ...

				double radius = 1e-3;
				size_t dimension = all_points[0].getDimension();

				boost::mt19937 generator;
				auto distribution = boost::uniform_real<>(0.0, 2.0 * boost::math::constants::pi<double>());

				std::vector<double> angles(dimension - 1);

				for (size_t i = 0; i < dimension - 1; ++i)
					angles[i] = distribution(generator);


				//std::vector<double> offset(dimension, radius);
				MathVector offset(std::vector<double>(dimension, radius));

				for (size_t coordinate = 0; coordinate < dimension; ++coordinate)
				{
					for (size_t i = 0; i < dimension - coordinate - 1; ++i)
						offset[coordinate] *= cos(angles[i]);

					if (coordinate != 0)
						offset[coordinate] *= sin(angles[dimension - coordinate - 1]);
				}

				// Get a moved point
				// -----
				//all_points[*it] += offset;
				all_points[*it] = all_points[*it] + offset;
				// -----
			}

			// =====
			//if (dot_v_a > 0)
			return *it;
			//else
			//	continue;
			// =====
		}

		ctg = -dot_v_a / abs_dot_v_n;
		// ~~~~~

		if (ctg < min_ctg)
		{
			min_ctg = ctg;
			index_of_point_with_min_ctg = *it;
		}

	}

	return index_of_point_with_min_ctg;
}

void GiftWrapping::create_coordinate_axis(size_t num_of_coordinate, size_t dimension, MathVector& coordinate_axis)
{
	coordinate_axis = std::vector<double>(dimension, 0.0);
	coordinate_axis[num_of_coordinate] = 1.0;
}


// Формально меняет all_points, но на самом деле нет
void GiftWrapping::find_first_hyperface(std::vector<MathVector>& all_points, std::unordered_set<size_t>& first_hyperface)
{
	// Initialize remaining indexes, which can be considered.
	std::unordered_set<size_t> indexes_of_candidates;
	for (size_t counter = 0; counter < all_points.size(); ++counter)
		indexes_of_candidates.insert(counter);

	// Find point with min first coordinate. First normal is (1, 0, ..., 0).
	double min_first_coordinate = all_points[0][0];
	size_t index_of_point_with_min_first_coordinate = 0;

	for (size_t counter = 1; counter < all_points.size(); ++counter)
		if (all_points[counter][0] < min_first_coordinate)
		{
			min_first_coordinate = all_points[counter][0];
			index_of_point_with_min_first_coordinate = counter;
		}

	first_hyperface.insert(index_of_point_with_min_first_coordinate);
	indexes_of_candidates.erase(index_of_point_with_min_first_coordinate);


	MathVector normal_of_subface;
	MathVector normal_of_hyperface;

	size_t dimension = all_points[0].getDimension();
	create_coordinate_axis(0, dimension, normal_of_hyperface);


	std::vector<MathVector> cross_product_vectors;
	MathVector coordinate_axis;
	size_t new_index;

	for (size_t counter_1 = 1; counter_1 < dimension; ++counter_1)
	{
		// Recalculate normal of a subface
		cross_product_vectors.clear();
		cross_product_vectors.push_back(normal_of_hyperface);

		auto it = std::next(first_hyperface.begin());

		for (size_t counter_2 = 1; counter_2 < counter_1; ++counter_2)
		{
			cross_product_vectors.push_back(all_points[*it] - all_points[*first_hyperface.begin()]);
			++it;
		}

		for (size_t counter_2 = counter_1 + 1; counter_2 < dimension; ++counter_2)
		{
			create_coordinate_axis(counter_2, dimension, coordinate_axis);
			cross_product_vectors.push_back(coordinate_axis);
		}

		normal_of_subface = MathVector::crossProduct(cross_product_vectors);
		normal_of_subface.normalize();

		new_index = wrapping(true, normal_of_hyperface, all_points[*first_hyperface.begin()], normal_of_subface, indexes_of_candidates, all_points);
		first_hyperface.insert(new_index);
		indexes_of_candidates.erase(new_index);

		// Recalculate normal of a hyperface
		cross_product_vectors.clear();

		it = std::next(first_hyperface.begin(), 1);
		for (size_t counter_2 = 1; counter_2 < counter_1 + 1; ++counter_2)
		{
			cross_product_vectors.push_back(all_points[*it] - all_points[*first_hyperface.begin()]);
			++it;
		}

		for (size_t counter_2 = counter_1 + 1; counter_2 < dimension; ++counter_2)
		{
			create_coordinate_axis(counter_2, dimension, coordinate_axis);
			cross_product_vectors.push_back(coordinate_axis);
		}

		// ~~~~~
		MathVector old_normal_of_hyperface = normal_of_hyperface;
		// ~~~~~

		normal_of_hyperface = MathVector::crossProduct(cross_product_vectors);

		// ~~~~~
		double eps = 1e-5;
		if (normal_of_hyperface * normal_of_hyperface < eps)
			normal_of_hyperface = old_normal_of_hyperface;
		else
			normal_of_hyperface.normalize();
		// ~~~~~
	}
}

