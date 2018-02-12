#pragma once

// -----
//#include <iostream>
// -----

struct TestConfig
{
	// Implicit casting enum class to true type (e.g. int)
	template <typename E>
	static constexpr auto to_underlying(E e) noexcept
	{
		return static_cast<std::underlying_type_t<E>>(e);
	}

	enum class test_2D
	{
		RegularPolygon,
		NonSimplicialSquare,
		LineSegment
	};

	enum class test_3D
	{
		Pyramid,
		Octahedron,
		Icosahedron,
		Cube,
		Dodecahedron,
		RegularPolygon,
		NonSimplicialSquare,
		LineSegment
	};

	enum class test_4D
	{
		Cell_5,
		Cell_16,
		Tesseract,
		Cell_24
	};

	TestConfig(	int	_b_random_seed,
				int	_dimension,
				int	_num_of_interior_points,
				int _test_id,
				int _num_of_vertices
			  )
	{
		is_correct =	(_b_random_seed >= 0)			&&
						(_dimension > 0)				&&
						(_num_of_interior_points >= 0)	&&
						(_test_id >= 0)					&&
						(!(_num_of_vertices > 2) ||
						( (_num_of_vertices > 2) && 
							(_dimension == 2 && _test_id == to_underlying(test_2D::RegularPolygon)) ||
							(_dimension == 3 && _test_id == to_underlying(test_3D::RegularPolygon))));


		// -----
		//std::cout << "is_correct = " << is_correct << std::endl;

		//std::cout << "_b_random_seed = " << _b_random_seed << std::endl;
		//std::cout << "_dimension = " << _dimension << std::endl;
		//std::cout << "_num_of_interior_points = " << _num_of_interior_points << std::endl;
		//std::cout << "_test_id = " << _test_id << std::endl;
		// -----

		if (is_correct)
		{
			b_random_seed = (_b_random_seed != 0);
			dimension = static_cast<size_t>(_dimension);
			num_of_interior_points = static_cast<size_t>(_num_of_interior_points);
			test_id = static_cast<size_t>(_test_id);
			
			if (_num_of_vertices > 2)
				num_of_vertices = static_cast<size_t>(_num_of_vertices);
			else
				num_of_vertices = 0;
		}
	}

	bool b_random_seed;
	size_t dimension;
	size_t num_of_interior_points;
	size_t test_id;
	size_t num_of_vertices;

	bool is_correct;

};
