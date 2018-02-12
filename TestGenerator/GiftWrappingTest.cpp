#include "../GiftWrapping/stdafx.h"
#include "GiftWrappingTest.h"

GiftWrappingTest::GiftWrappingTest(TestConfig _cfg) : cfg(_cfg)
{
}


GiftWrappingTest::~GiftWrappingTest()
{
}

void GiftWrappingTest::generate_test()
{
	switch (cfg.dimension)
	{
	case 2:
		switch (cfg.test_id)
		{
		case TestConfig::to_underlying(TestConfig::test_2D::RegularPolygon):
			testRegularPolygon_2D(cfg.num_of_vertices, cfg.num_of_vertices);
			break;
		case TestConfig::to_underlying(TestConfig::test_2D::NonSimplicialSquare):
			testNonSimplicialSquare_2D(cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_2D::LineSegment):
			testLineSegment_2D(true);
			break;
		default:
			return;
		}
		break;

	case 3:
		switch (cfg.test_id)
		{
		case TestConfig::to_underlying(TestConfig::test_3D::Pyramid):
			testPyramid(cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_3D::Octahedron):
			testOctahedron(cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_3D::Icosahedron):
			testIcosahedron(cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_3D::Cube):
			testCube(cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_3D::Dodecahedron):
			testDodecahedron(cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_3D::RegularPolygon):
			testRegularPolygon_2D_in_3D(cfg.num_of_vertices, cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_3D::NonSimplicialSquare):
			testNonSimplicialSquare_2D_in_3D(cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_3D::LineSegment):
			testLineSegment_2D_in_3D(true);
			break;
		default:
			return;
		}
		break;

	case 4:
		switch (cfg.test_id)
		{
		case TestConfig::to_underlying(TestConfig::test_4D::Cell_5):
			test5Cell(cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_4D::Cell_16):
			test16Cell(cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_4D::Tesseract):
			testTesseract(cfg.num_of_interior_points);
			break;
		case TestConfig::to_underlying(TestConfig::test_4D::Cell_24):
			test24Cell(cfg.num_of_interior_points);
			break;
		default:
			return;
		}
		break;

	default:
		return;
	}

	// -----
	std::ofstream out_stream("data/source_points.txt");

	out_stream << cfg.dimension << std::endl;

	for (auto point : result)
		out_stream << point << std::endl;

	out_stream.close();
	// -----
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

void GiftWrappingTest::setFilledPolyhedron(	size_t num_of_interior_points, 
											const std::vector<MathVector>& vertices, 
											const std::vector<std::vector<MathVector>>& hyperfaces,
											size_t rnd_seed,
											std::vector<MathVector>& result)
{
	size_t dimension = vertices.begin()->getDimension();

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

	boost::mt19937 generator;
	generator.seed(rnd_seed);
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
				result.push_back(interior_point);
				break;
			}

		}
	}

}

void GiftWrappingTest::testPolyhedron(	size_t num_of_interior_points, 
										const std::vector<MathVector>& vertices, 
										const std::vector<std::vector<MathVector>>& hyperfaces, 
										void(*postprocessing_points)(std::vector<MathVector>& points))
{
	result = vertices;

	// -----
	size_t rnd_seed = 0;
	if (cfg.b_random_seed)
		rnd_seed = static_cast<size_t>(std::time(nullptr));
	// -----

	setFilledPolyhedron(num_of_interior_points, vertices, hyperfaces, rnd_seed, result);

	if (postprocessing_points != nullptr)
		postprocessing_points(result);
}


void GiftWrappingTest::testRandomPointCloud(size_t num_of_points)
{

}

void GiftWrappingTest::testRegularPolygon_2D(size_t num_of_vertices, size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;
	std::vector<std::vector<MathVector>> hyperfaces;

	generate_regular_polygon_2D(num_of_vertices, num_of_interior_points, vertices, hyperfaces);

	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
	//testNonSymplicialAlgorithm(num_of_interior_points, vertices, hyperfaces);
}

void GiftWrappingTest::testRegularPolygon_2D_in_3D(size_t num_of_vertices, size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;
	std::vector<std::vector<MathVector>> hyperfaces;

	generate_regular_polygon_2D(num_of_vertices, num_of_interior_points, vertices, hyperfaces);

	testPolyhedron(num_of_interior_points, vertices, hyperfaces, 
	[](std::vector<MathVector>& points) 
	{
		std::vector<MathVector> new_points;
		MathVector new_p;

		size_t dimension = points[0].getDimension();

		for (auto p : points)
		{
			new_p = MathVector(dimension + 1);

			for (size_t i = 0; i < dimension; ++i)
				new_p[i] = p[i];

			new_p[dimension] = 0.0;

			new_points.push_back(new_p);
		}

		rotate_translate(new_points, { boost::math::double_constants::pi / 4, 0.0, 0.0 }, MathVector({ 0.0, 0.0, 0.0 }));

		points = new_points;

	});
}

void GiftWrappingTest::testNonSimplicialSquare_2D(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({ -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0,  0.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0 }));
	vertices.push_back(MathVector({  0.0,  1.0 }));
	vertices.push_back(MathVector({  1.0,  1.0 }));
	vertices.push_back(MathVector({  1.0,  0.0 }));
	vertices.push_back(MathVector({  1.0, -1.0 }));
	vertices.push_back(MathVector({  0.0, -1.0 }));

	std::vector<std::vector<MathVector>> hyperfaces;

	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[1] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[4] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[5], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[7], vertices[0] }));

	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
}

void GiftWrappingTest::testNonSimplicialSquare_2D_in_3D(size_t num_of_interior_points)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({ -1.0, -1.0 }));
	vertices.push_back(MathVector({ -1.0,  0.0 }));
	vertices.push_back(MathVector({ -1.0,  1.0 }));
	vertices.push_back(MathVector({ 0.0,  1.0 }));
	vertices.push_back(MathVector({ 1.0,  1.0 }));
	vertices.push_back(MathVector({ 1.0,  0.0 }));
	vertices.push_back(MathVector({ 1.0, -1.0 }));
	vertices.push_back(MathVector({ 0.0, -1.0 }));

	std::vector<std::vector<MathVector>> hyperfaces;

	hyperfaces.push_back(std::vector<MathVector>({ vertices[0], vertices[1] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[1], vertices[2] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[2], vertices[3] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[3], vertices[4] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[4], vertices[5] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[5], vertices[6] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[6], vertices[7] }));
	hyperfaces.push_back(std::vector<MathVector>({ vertices[7], vertices[0] }));

	testPolyhedron(num_of_interior_points, vertices, hyperfaces, 
		[](std::vector<MathVector>& points)
		{
			std::vector<MathVector> new_points;
			MathVector new_p;

			size_t dimension = points[0].getDimension();

			for (auto p : points)
			{
				new_p = MathVector(dimension + 1);

				for (size_t i = 0; i < dimension; ++i)
					new_p[i] = p[i];

				new_p[dimension] = 0.0;

				new_points.push_back(new_p);
			}

			rotate_translate(new_points, { boost::math::double_constants::pi / 4, 0.0, 0.0 }, MathVector({ 0.0, 0.0, 0.0 }));

			points = new_points;

		});
}

void GiftWrappingTest::testLineSegment_2D(bool add_point_in_the_middle)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({ -1.0, -1.0 }));
	vertices.push_back(MathVector({  1.0,  1.0 }));

	if (add_point_in_the_middle)
		vertices.push_back(MathVector({ 0.0,  0.0 }));

	std::vector<std::vector<MathVector>> hyperfaces;

	testPolyhedron(0, vertices, hyperfaces, nullptr);
}

void GiftWrappingTest::testLineSegment_2D_in_3D(bool add_point_in_the_middle)
{
	std::vector<MathVector> vertices;

	vertices.push_back(MathVector({ -1.0, -1.0 }));
	vertices.push_back(MathVector({ 1.0,  1.0 }));

	if (add_point_in_the_middle)
		vertices.push_back(MathVector({ 0.0,  0.0 }));

	std::vector<std::vector<MathVector>> hyperfaces;

	testPolyhedron(0, vertices, hyperfaces, 
		[](std::vector<MathVector>& points)
		{
			std::vector<MathVector> new_points;
			MathVector new_p;

			size_t dimension = points[0].getDimension();

			for (auto p : points)
			{
				new_p = MathVector(dimension + 1);

				for (size_t i = 0; i < dimension; ++i)
					new_p[i] = p[i];

				new_p[dimension] = 0.0;

				new_points.push_back(new_p);
			}

			rotate_translate(new_points, { boost::math::double_constants::pi / 4, 0.0, 0.0 }, MathVector({ 0.0, 0.0, 0.0 }));

			points = new_points;

		});
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

	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
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

	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
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

	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
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

	//testNonSymplicialAlgorithm(num_of_interior_points, vertices, hyperfaces);
	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
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

	//testNonSymplicialAlgorithm(num_of_interior_points, vertices, hyperfaces);
	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
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

	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
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

	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
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

	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
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
	
	//testNonSymplicialAlgorithm(num_of_interior_points, vertices, hyperfaces);
	testPolyhedron(num_of_interior_points, vertices, hyperfaces, nullptr);
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

void GiftWrappingTest::rotate_translate(std::vector<MathVector>& points, const std::vector<double>& angles, const MathVector & offset)
{
	std::vector<std::vector<double>> rotation_matrix;
	size_t dimension = points[0].getDimension();

	for (auto angle : angles)
		for (size_t axis_1 = 0; axis_1 < dimension; ++axis_1)
			for (size_t axis_2 = axis_1 + 1; axis_2 < dimension; ++axis_2)
			{
				generate_rotation_matrix(angle, axis_1, axis_2, dimension, rotation_matrix);
				
				for (size_t i = 0; i < points.size(); ++i)
					points[i] = rotation_matrix * points[i];
			}

	for (size_t i = 0; i < points.size(); ++i)
		points[i] = points[i] + offset;

}

void GiftWrappingTest::generate_rotation_matrix(double angle, size_t axis_1, size_t axis_2, size_t dimension, std::vector<std::vector<double>>& res_matrix)
{
	res_matrix.clear();
	res_matrix.assign(dimension, std::vector<double>());

	for (size_t i = 0; i < res_matrix.size(); ++i)
		res_matrix[i].assign(dimension, 0);	

	for (size_t i = 0; i < dimension; ++i)
		res_matrix[i][i] = 1.0;

	res_matrix[axis_1][axis_1] =  cos(angle);
	res_matrix[axis_2][axis_2] =  cos(angle);

	res_matrix[axis_1][axis_2] = -sin(angle);
	res_matrix[axis_2][axis_1] =  sin(angle);
}

void GiftWrappingTest::generate_regular_polygon_2D(size_t num_of_vertices, size_t num_of_interior_points, std::vector<MathVector>& vertices, std::vector<std::vector<MathVector>>& hyperfaces)
{
	const double angle_offset = 0.0;
	const double scale_factor = 1.0;

	vertices.clear();
	hyperfaces.clear();

	for (size_t counter = 0; counter < num_of_vertices; ++counter)
		vertices.push_back(MathVector({ cos((angle_offset + 2 * boost::math::double_constants::pi * counter) / num_of_vertices),
			sin((angle_offset + 2 * boost::math::double_constants::pi * counter) / num_of_vertices) }));

	for (size_t counter = 0; counter < num_of_vertices - 1; ++counter)
		hyperfaces.push_back(std::vector<MathVector>({ vertices[counter], vertices[counter + 1] }));

	hyperfaces.push_back(std::vector<MathVector>({ vertices[num_of_vertices - 1], vertices[0] }));

}