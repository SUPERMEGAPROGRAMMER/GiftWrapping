#pragma once

//#include "GiftWrapping.h"
#include "MathVector.h"
#include "TestConfig.h"
#include <vector>
// -----
#include <fstream>
// -----


class GiftWrappingTest
{
public:
	GiftWrappingTest(TestConfig cfg);
	~GiftWrappingTest();

	// -----
	void generate_test();
	
	std::vector<MathVector> result;
	// -----

private:
	void testRegularPolygon_2D(size_t num_of_vertices, size_t num_of_interior_points);
	void testRegularPolygon_2D_in_3D(size_t num_of_vertices, size_t num_of_interior_points);
	void testNonSimplicialSquare_2D(size_t num_of_interior_points);
	void testNonSimplicialSquare_2D_in_3D(size_t num_of_interior_points);
	void testLineSegment_2D(bool add_point_in_the_middle);
	void testLineSegment_2D_in_3D(bool add_point_in_the_middle);

	void testPyramid(size_t num_of_interior_points);
	void testOctahedron(size_t num_of_interior_points);
	void testIcosahedron(size_t num_of_interior_points);
	void testCube(size_t num_of_interior_points);
	void testDodecahedron(size_t num_of_interior_points);

	void test5Cell(size_t num_of_interior_points);
	void test16Cell(size_t num_of_interior_points);
	void testTesseract(size_t num_of_interior_points);
	void test24Cell(size_t num_of_interior_points);
	void testPenterakt(size_t num_of_interior_points);


	void testPolyhedron(size_t num_of_interior_points, const std::vector<MathVector>& vertices, const std::vector<std::vector<MathVector>>& hyperfaces, void(*postprocessing_points)(std::vector<MathVector>& points));
	
	static int signOfSemiSpace(const std::vector<MathVector>& points_of_hyperface, const MathVector& test_point);
	static void testRandomPointCloud(size_t num_of_points);
	static void setFilledPolyhedron(	size_t num_of_interior_points, 
								const std::vector<MathVector>& vertices, 
								const std::vector<std::vector<MathVector>>& hyperfaces, 
								size_t rnd_seed,
								std::vector<MathVector>& result);
	static void rotate_translate(std::vector<MathVector>& points, const std::vector<double>& angles, const MathVector& offset);
	static void generate_rotation_matrix(double angle, size_t axis_1, size_t axis_2, size_t dimension, std::vector<std::vector<double>>& res_matrix);
	static void generate_regular_polygon_2D(size_t num_of_vertices, size_t num_of_interior_points, std::vector<MathVector>& vertices, std::vector<std::vector<MathVector>>& hyperfaces);

	// -----
	TestConfig cfg;
	// -----
};

