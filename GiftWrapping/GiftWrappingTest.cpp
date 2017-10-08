#include "stdafx.h"
#include "GiftWrappingTest.h"

GiftWrappingTest::GiftWrappingTest()
{
}


GiftWrappingTest::~GiftWrappingTest()
{
}

int GiftWrappingTest::signOfSemiSpace(const std::vector<MathVector>& points_of_hyperface, const MathVector& test_point)
{
	std::vector<MathVector> cross_product_vectors;
	for (auto it = std::next(points_of_hyperface.begin(), 1); it != points_of_hyperface.end(); ++it)
		cross_product_vectors.push_back(*it - *points_of_hyperface.begin());

	MathVector coefficients = MathVector::crossProduct(cross_product_vectors);

	double res = 0.0;
	for (size_t counter = 0; counter < test_point.getDimension(); ++counter)
		res += coefficients[counter] * (test_point[counter] - (*points_of_hyperface.begin())[counter]);

	return res > 0 ? 1 : res < 0 ? -1 : 0;
}

// -----
void GiftWrappingTest::setFilledPolyhedron(size_t num_of_interior_points, const std::vector<MathVector>& vertices, const std::vector<std::vector<MathVector>>& hyperfaces, std::vector<MathVector>& result)
{
	size_t dimension = vertices.begin()->getDimension();

	// -----
	// Find MIN and MAX of each coordinate.

	std::vector<double> min_coordinate(dimension);
	std::vector<double> max_coordinate(dimension);

	for (size_t coordinate = 0; coordinate < dimension; ++coordinate)
	{
		min_coordinate[coordinate] = max_coordinate[coordinate] = (*result.begin())[coordinate];//(*gw.input_points.begin())[coordinate];

		for (auto point : result)
		{
			if (point[coordinate] < min_coordinate[coordinate])
				min_coordinate[coordinate] = point[coordinate];
			else
				if (point[coordinate] > max_coordinate[coordinate])
					max_coordinate[coordinate] = point[coordinate];
		}
	}

	// -----

	boost::mt19937 generator;
	generator.seed(static_cast<unsigned>(std::time(0)));
	std::vector<boost::uniform_real<>> distributions(dimension);
	for (size_t coordinate = 0; coordinate < dimension; ++coordinate)
		distributions[coordinate] = boost::uniform_real<>(min_coordinate[coordinate], max_coordinate[coordinate]);

	MathVector interior_point(dimension);
	std::vector<int> signs;

	for (size_t counter_1 = 0; counter_1 < num_of_interior_points; ++counter_1)
	{
		while (true)
		{
			for (size_t coordinate = 0; coordinate < dimension; ++coordinate)
				interior_point[coordinate] = distributions[coordinate](generator);


			for (auto hyperface : hyperfaces)
				signs.push_back(signOfSemiSpace(hyperface, interior_point));

			// -----
			// Мне не нравится такой подход (поискать в будущем что-то более высокоуровневое из std::algorithm (а-ля алгоритмы stl))
			bool isEqualSigns = true;

			for (auto sign : signs)
				if (sign * signs[0] < 0)
				{
					isEqualSigns = false;
					break;
				}

			signs.clear();
			// -----

			if (isEqualSigns)
			{
				// =====
				std::cout << interior_point << std::endl;
				// =====

				result.push_back(interior_point);
				break;
			}

		}
	}

}
// -----
//size_t us_hash(const std::unordered_set<size_t>& S);
/*
size_t us_hash(const std::unordered_set<size_t>& S)
{
	size_t sum = 0;
	for (auto item : S)
		sum += item;
	return sum;
}
*/

void GiftWrappingTest::testNonSymplicialAlgorithm(size_t num_of_interior_points, const std::vector<MathVector>& vertices, const std::vector<std::vector<MathVector>>& hyperfaces)
{
	const size_t num_of_iter = 10;
	
	auto gw = GiftWrapping();
	gw.input_points = vertices;
	setFilledPolyhedron(num_of_interior_points, vertices, hyperfaces, gw.input_points);

	std::vector<size_t> nums_of_selection(gw.input_points.size(), 0);
	std::unordered_set<size_t> selected_indexes(0);
	
	// ~~~~~
	//auto nums_convex_hulls = std::map<std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)>, size_t>();
	// ~~~~~

	for (size_t counter = 0; counter < num_of_iter; ++counter)
	{
		gw.scatter_points.clear();
		gw.run_algorithm();

		selected_indexes.clear();
		for (auto hyperface : gw.convex_hull)
			for (auto i : hyperface)
			{
				selected_indexes.insert(i);
			}
		for (auto i : selected_indexes)
			++nums_of_selection[i];

		// ~~~~~
		/*
		if (nums_convex_hulls.count(gw.convex_hull))
			++nums_convex_hulls[gw.convex_hull];
		else
			nums_convex_hulls.insert(std::pair<std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)>, size_t>(gw.convex_hull, 0));
		*/
		// ~~~~~
	}

	// ~~~~~
	// find the most popular convex hull
	/*
	auto max_iter = nums_convex_hulls.begin();
	for (auto it = nums_convex_hulls.begin(); it != nums_convex_hulls.end(); ++it)
		if (it->second > max_iter->second)
			max_iter = it;
	*/
	// ~~~~~

	std::ofstream out_points("data\\points.txt");
	for (auto point : gw.input_points)
		out_points << point << std::endl;
	out_points.close();
	
	std::ofstream out_hyperfaces("data\\faces.txt");
	for (auto hyperface : gw.convex_hull)
	//for (auto hyperface : max_iter->first)
	{
		for (auto index : hyperface)
			out_hyperfaces << index << " ";
		out_hyperfaces << std::endl;
	}
	out_hyperfaces.close();

	
	std::ofstream out_stats_info("data\\stats_info.txt");
	for (auto num : nums_of_selection)
		out_stats_info << num << std::endl;
	out_stats_info.close();

	system("python draw_stats_CH.py");
}

void GiftWrappingTest::testPolyhedron(size_t num_of_interior_points, const std::vector<MathVector>& vertices, const std::vector<std::vector<MathVector>>& hyperfaces)
{
	auto gw = GiftWrapping();

	gw.input_points = vertices;

	// -----
	setFilledPolyhedron(num_of_interior_points, vertices, hyperfaces, gw.input_points);
	// -----

	gw.run_algorithm();

	// -----
	std::ofstream out_points("data\\points.txt");

	// -----
	//for (auto point : gw.input_points)
	for (auto point : gw.scatter_points)
		out_points << point << std::endl;
	// -----

	out_points.close();


	std::ofstream out_hyperfaces("data\\faces.txt");

	for (auto hyperface : gw.convex_hull)
	{
		for (auto index : hyperface)
			out_hyperfaces << index << " ";

		out_hyperfaces << std::endl;
	}

	out_hyperfaces.close();

	// -----

	// $$$$$
	for (auto hyperface : gw.convex_hull)
	{
		std::cout << "{ ";
		for (auto index : hyperface)
			std::cout << index << " ";
		std::cout << "}" << std::endl;
	}
	// $$$$$

	system("python plot_3D4D.py");
	//system("python draw_stats_CH.py");
}

void GiftWrappingTest::testRandomPointCloud(size_t num_of_points)
{

}

void GiftWrappingTest::testRegularPolygon2D(size_t num_of_vertices, size_t num_of_interior_points)
{
	const double angle_offset = 0.0;
	const double scale_factor = 1.0;


	std::vector<MathVector> vertices;

	for (size_t counter = 0; counter < num_of_vertices; ++counter)
		vertices.push_back(MathVector({cos((angle_offset + 2 * boost::math::double_constants::pi * counter) / num_of_vertices), 
									   sin((angle_offset + 2 * boost::math::double_constants::pi * counter) / num_of_vertices)}));


	std::vector<std::vector<MathVector>> hyperfaces;

	for (size_t counter = 0; counter < num_of_vertices - 1; ++counter)
		hyperfaces.push_back(std::vector<MathVector>({vertices[counter], vertices[counter + 1]}));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[num_of_vertices - 1], vertices[0] }));


	testPolyhedron(num_of_interior_points, vertices, hyperfaces);
}

void GiftWrappingTest::testPyramid(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;
	vertices.push_back(MathVector({ 0.0, 0.0, 1.0 }));
	vertices.push_back(MathVector({ -0.5, pow(3, 0.5) / 2.0, 0.0 }));
	vertices.push_back(MathVector({ -0.5, -pow(3, 0.5) / 2.0, 0.0 }));
	vertices.push_back(MathVector({ 1.0, 0.0, 0.0 }));

	std::vector<std::vector<MathVector>> hyperfaces;
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[1], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[3], vertices[1] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[2], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[1], vertices[3] }));

	testPolyhedron(num_of_interior_points, vertices, hyperfaces);
} 

void GiftWrappingTest::testOctahedron(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;
	vertices.push_back(MathVector({ 1.0, 0.0, 0.0 }));
	vertices.push_back(MathVector({ -1.0, 0.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0, 1.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0, -1.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0, 0.0, 1.0 }));
	vertices.push_back(MathVector({ 0.0, 0.0, -1.0 }));

	std::vector<std::vector<MathVector>> hyperfaces;
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[4], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[4], vertices[0] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[0], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[2], vertices[5] }));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[1], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[3], vertices[0] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[5], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[5], vertices[0] }));

	testPolyhedron(num_of_interior_points, vertices, hyperfaces);
}

void GiftWrappingTest::testIcosahedron(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({ -0.692, 0.000, 0.427 }));
	vertices.push_back(MathVector({ 0.000, 0.427, -0.692 }));
	vertices.push_back(MathVector({ 0.000, 0.427, 0.692 }));
	vertices.push_back(MathVector({ 0.692, 0.000, -0.427 }));
	vertices.push_back(MathVector({ -0.427, -0.692, 0.000 }));
	vertices.push_back(MathVector({ -0.427, 0.692, 0.000 }));
	vertices.push_back(MathVector({ 0.000, -0.427, 0.692 }));
	vertices.push_back(MathVector({ 0.427, 0.692, 0.000 }));
	vertices.push_back(MathVector({ 0.000, -0.427, -0.692 }));
	vertices.push_back(MathVector({ 0.692, 0.000, 0.427 }));
	vertices.push_back(MathVector({ 0.427, -0.692, 0.000 }));
	vertices.push_back(MathVector({ -0.692, 0.000, -0.427 }));

	std::vector<std::vector<MathVector>> hyperfaces;

	hyperfaces.push_back(std::vector<MathVector>({ vertices[9], vertices[2], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[11], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[11], vertices[1], vertices[8] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[11], vertices[4] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[1], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[8], vertices[1] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[9], vertices[3], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[6], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[10], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[5], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[7], vertices[5], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[8], vertices[3], vertices[10] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[11], vertices[8] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[9], vertices[7], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[10], vertices[9], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[5], vertices[11] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[2], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[8], vertices[10], vertices[4] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[9], vertices[10] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[0], vertices[4] }));

	testPolyhedron(num_of_interior_points, vertices, hyperfaces);
}

void GiftWrappingTest::testCube(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({ -1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ 1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ 1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({ 1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ 1.0,  1.0,  1.0 }));

	std::vector<std::vector<MathVector>> hyperfaces;


	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[2], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[7], vertices[2] }));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[7], vertices[3], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[5], vertices[3] }));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[5], vertices[1], vertices[4] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[4], vertices[1] }));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[0], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[6], vertices[0] }));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[6], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[7], vertices[5], vertices[6] }));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[1], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[2], vertices[1] }));

	testNonSymplicialAlgorithm(num_of_interior_points, vertices, hyperfaces);
	//testPolyhedron(num_of_interior_points, vertices, hyperfaces);
}

void GiftWrappingTest::testDodecahedron(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({  0.469,  0.469,  0.469 }));
	vertices.push_back(MathVector({  0.290,  0.000,  0.759 }));
	vertices.push_back(MathVector({ -0.759, -0.290,  0.000 }));
	vertices.push_back(MathVector({  0.759,  0.290,  0.000 }));
	vertices.push_back(MathVector({ -0.469,  0.469, -0.469 }));

	vertices.push_back(MathVector({  0.000, -0.759, -0.290 }));
	vertices.push_back(MathVector({ -0.759,  0.290,  0.000 }));
	vertices.push_back(MathVector({  0.469, -0.469,  0.469 }));
	vertices.push_back(MathVector({ -0.469,  0.469,  0.469 }));
	vertices.push_back(MathVector({ -0.469, -0.469,  0.469 }));

	vertices.push_back(MathVector({  0.469, -0.469, -0.469 }));
	vertices.push_back(MathVector({  0.290,  0.000, -0.759 }));
	vertices.push_back(MathVector({ -0.469, -0.469, -0.469 }));
	vertices.push_back(MathVector({  0.000, -0.759,  0.290 }));
	vertices.push_back(MathVector({  0.000,  0.759, -0.290 }));

	vertices.push_back(MathVector({ -0.290,  0.000,  0.759 }));
	vertices.push_back(MathVector({  0.759, -0.290,  0.000 }));
	vertices.push_back(MathVector({ -0.290,  0.000, -0.759 }));
	vertices.push_back(MathVector({  0.469,  0.469, -0.469 }));
	vertices.push_back(MathVector({  0.000,  0.759,  0.290 }));

	std::vector<std::vector<MathVector>> hyperfaces;

	hyperfaces.push_back(std::vector<MathVector>({ vertices[9], vertices[15], vertices[1], vertices[7], vertices[13] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[4], vertices[14], vertices[19], vertices[8] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[12], vertices[2], vertices[9], vertices[13], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[2], vertices[12], vertices[17], vertices[4] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[16], vertices[3], vertices[18], vertices[11], vertices[10] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[19], vertices[0], vertices[1], vertices[15], vertices[8] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[16], vertices[7], vertices[1], vertices[0], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[5], vertices[10], vertices[11], vertices[17], vertices[12] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[18], vertices[14], vertices[4], vertices[17], vertices[11] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[16], vertices[10], vertices[5], vertices[13], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[6], vertices[8], vertices[15], vertices[9] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[19], vertices[14], vertices[18], vertices[3], vertices[0] }));

	testNonSymplicialAlgorithm(num_of_interior_points, vertices, hyperfaces);
	//testPolyhedron(num_of_interior_points, vertices, hyperfaces);
}

void GiftWrappingTest::test5Cell(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({ 1.0,  1.0,  1.0, 0.0 }));
	vertices.push_back(MathVector({ 1.0, -1.0, -1.0, 0.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0, -1.0, 0.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0,  1.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0,  0.0,  0.0, sqrt(5.0) }));


	std::vector<std::vector<MathVector>> hyperfaces;

	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[0], vertices[2], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[0], vertices[2], vertices[1] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[0], vertices[1], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[0], vertices[3], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[1], vertices[2], vertices[3] }));

	testPolyhedron(num_of_interior_points, vertices, hyperfaces);
}

void GiftWrappingTest::test16Cell(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({ 1.0,  0.0,  0.0, 0.0 }));
	vertices.push_back(MathVector({ -1.0,  0.0,  0.0, 0.0 }));

	vertices.push_back(MathVector({ 0.0,  1.0,  0.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0, -1.0,  0.0, 0.0 }));

	vertices.push_back(MathVector({ 0.0,  0.0,  1.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0,  0.0, -1.0, 0.0 }));

	vertices.push_back(MathVector({ 0.0,  0.0,  0.0,  1.0 }));
	vertices.push_back(MathVector({ 0.0,  0.0,  0.0, -1.0 }));


	std::vector<std::vector<MathVector>> hyperfaces;


	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[5], vertices[1], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[5], vertices[2], vertices[0] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[5], vertices[0], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[5], vertices[3], vertices[1] }));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[4], vertices[2], vertices[1] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[4], vertices[0], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[4], vertices[3], vertices[0] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[4], vertices[1], vertices[3] }));


	hyperfaces.push_back(std::vector<MathVector>({ vertices[5], vertices[7], vertices[1], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[5], vertices[7], vertices[2], vertices[0] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[5], vertices[7], vertices[0], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[5], vertices[7], vertices[3], vertices[1] }));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[7], vertices[2], vertices[1] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[7], vertices[0], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[7], vertices[3], vertices[0] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[7], vertices[1], vertices[3] }));

	testPolyhedron(num_of_interior_points, vertices, hyperfaces);
}

void GiftWrappingTest::testTesseract(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({ -1.0, -1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({ 1.0, -1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ 1.0, -1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ 1.0, -1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ 1.0, -1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({ 1.0,  1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ 1.0,  1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ 1.0,  1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ 1.0,  1.0,  1.0,  1.0 }));

	std::vector<std::vector<MathVector>> hyperfaces;

	// Inner cube
	hyperfaces.push_back(std::vector<MathVector>({ vertices[10], vertices[8], vertices[12], vertices[14] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[14], vertices[12], vertices[4], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[4], vertices[0], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[0], vertices[8], vertices[10] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[10], vertices[14], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[8], vertices[0], vertices[4], vertices[12] }));

	// Outer cube
	hyperfaces.push_back(std::vector<MathVector>({ vertices[11], vertices[9], vertices[13], vertices[15] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[15], vertices[13], vertices[5], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[7], vertices[5], vertices[1], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[1], vertices[9], vertices[11] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[11], vertices[15], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[9], vertices[1], vertices[5], vertices[13] }));

	// Bottom partitions
	hyperfaces.push_back(std::vector<MathVector>({ vertices[8], vertices[9], vertices[13], vertices[12] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[12], vertices[13], vertices[5], vertices[4] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[5], vertices[1], vertices[0] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[1], vertices[9], vertices[8] }));

	// Upper partitions
	hyperfaces.push_back(std::vector<MathVector>({ vertices[10], vertices[11], vertices[15], vertices[14] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[14], vertices[15], vertices[7], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[7], vertices[3], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[3], vertices[11], vertices[10] }));

	// Side partitions
	hyperfaces.push_back(std::vector<MathVector>({ vertices[11], vertices[10], vertices[8], vertices[9] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[15], vertices[14], vertices[12], vertices[13] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[7], vertices[6], vertices[4], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[2], vertices[0], vertices[1] }));

	testPolyhedron(num_of_interior_points, vertices, hyperfaces);
	//testNonSymplicialAlgorithm(num_of_interior_points, vertices, hyperfaces);
}

void GiftWrappingTest::test24Cell(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({  2.0,  0.0,  0.0, 0.0 }));
	vertices.push_back(MathVector({ -2.0,  0.0,  0.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0,  2.0,  0.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0, -2.0,  0.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0,  0.0,  2.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0,  0.0, -2.0, 0.0 }));
	vertices.push_back(MathVector({ 0.0,  0.0,  0.0,  2.0 }));
	vertices.push_back(MathVector({ 0.0,  0.0,  0.0, -2.0 }));

	vertices.push_back(MathVector({ -1.0, -1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0,  1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0,  1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0,  1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0,  1.0,  1.0,  1.0 }));

	std::vector<std::vector<MathVector>> hyperfaces;


	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[16], vertices[17], vertices[19], vertices[18], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[22], vertices[23], vertices[21], vertices[20], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[20], vertices[21], vertices[17], vertices[16], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[20], vertices[16], vertices[18], vertices[22], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[17], vertices[21], vertices[23], vertices[19], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[18], vertices[19], vertices[23], vertices[22], vertices[4] }));
	
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[8], vertices[10], vertices[11], vertices[9], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[12], vertices[8], vertices[9], vertices[13], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[14], vertices[10], vertices[8], vertices[12], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[9], vertices[11], vertices[15], vertices[13], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[11], vertices[10], vertices[14], vertices[15], vertices[4] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[14], vertices[12], vertices[13], vertices[15], vertices[2] }));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[12], vertices[13], vertices[21], vertices[20], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[14], vertices[12], vertices[20], vertices[22], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[13], vertices[15], vertices[23], vertices[21], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[15], vertices[14], vertices[22], vertices[23], vertices[4] }));

	
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[8], vertices[16], vertices[17], vertices[9], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[10], vertices[18], vertices[16], vertices[8], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[9], vertices[17], vertices[19], vertices[11], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[11], vertices[19], vertices[18], vertices[10], vertices[4] }));

	
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[10], vertices[14], vertices[22], vertices[18], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[11], vertices[19], vertices[23], vertices[15], vertices[6] }));
	
	
	hyperfaces.push_back(std::vector<MathVector>({ vertices[5], vertices[8], vertices[16], vertices[20], vertices[12], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[5], vertices[9], vertices[13], vertices[21], vertices[17], vertices[6] }));
	
	testNonSymplicialAlgorithm(num_of_interior_points, vertices, hyperfaces);
	//testPolyhedron(num_of_interior_points, vertices, hyperfaces);
}

void GiftWrappingTest::testPenterakt(size_t num_of_interior_points)
{
	/*
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({ -1.0, -1.0, -1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0, -1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0, -1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0, -1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0,  1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0,  1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0,  1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, -1.0,  1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0, 1.0, -1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, 1.0, -1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0, 1.0, -1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, 1.0, -1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0, 1.0,  1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, 1.0,  1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({ -1.0, 1.0,  1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0, 1.0,  1.0,  1.0,  1.0 }));

	vertices.push_back(MathVector({  1.0, -1.0, -1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0, -1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0, -1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0, -1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0,  1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0,  1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0,  1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0, -1.0,  1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0, 1.0, -1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0, 1.0, -1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0, 1.0, -1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0, 1.0, -1.0,  1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0, 1.0,  1.0, -1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0, 1.0,  1.0, -1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0, 1.0,  1.0,  1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0, 1.0,  1.0,  1.0,  1.0 }));


	std::vector<std::vector<MathVector>> hyperfaces;

	// Inner cube
	hyperfaces.push_back(std::vector<MathVector>({ vertices[10], vertices[8], vertices[12], vertices[14] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[14], vertices[12], vertices[4], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[4], vertices[0], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[0], vertices[8], vertices[10] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[10], vertices[14], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[8], vertices[0], vertices[4], vertices[12] }));

	// Outer cube
	hyperfaces.push_back(std::vector<MathVector>({ vertices[11], vertices[9], vertices[13], vertices[15] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[15], vertices[13], vertices[5], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[7], vertices[5], vertices[1], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[1], vertices[9], vertices[11] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[11], vertices[15], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[9], vertices[1], vertices[5], vertices[13] }));

	// Bottom partitions
	hyperfaces.push_back(std::vector<MathVector>({ vertices[8], vertices[9], vertices[13], vertices[12] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[12], vertices[13], vertices[5], vertices[4] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[5], vertices[1], vertices[0] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[1], vertices[9], vertices[8] }));

	// Upper partitions
	hyperfaces.push_back(std::vector<MathVector>({ vertices[10], vertices[11], vertices[15], vertices[14] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[14], vertices[15], vertices[7], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[7], vertices[3], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[3], vertices[11], vertices[10] }));

	// Side partitions
	hyperfaces.push_back(std::vector<MathVector>({ vertices[11], vertices[10], vertices[8], vertices[9] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[15], vertices[14], vertices[12], vertices[13] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[7], vertices[6], vertices[4], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[2], vertices[0], vertices[1] }));

	testPolyhedron(num_of_interior_points, vertices, hyperfaces);
	*/
}