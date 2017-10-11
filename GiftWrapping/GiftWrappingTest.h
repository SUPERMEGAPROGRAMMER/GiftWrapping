#pragma once

#include "GiftWrapping.h"
#include "MathVector.h"

class GiftWrappingTest
{
public:
	GiftWrappingTest();
	~GiftWrappingTest();

	// -----
	static void testRegularPolygon_2D(size_t num_of_vertices, size_t num_of_interior_points);
	static void testRegularPolygon_2D_in_3D(size_t num_of_vertices, size_t num_of_interior_points);
	static void testNonSimplicialSquare_2D(size_t num_of_interior_points);
	static void testNonSimplicialSquare_2D_in_3D(size_t num_of_interior_points);
	static void testLineSegment_2D(bool add_point_in_the_middle);
	static void testLineSegment_2D_in_3D(bool add_point_in_the_middle);
	// -----

	static void testPyramid(size_t num_of_interior_points);
	static void testOctahedron(size_t num_of_interior_points);
	static void testIcosahedron(size_t num_of_interior_points);
	static void testCube(size_t num_of_interior_points);
	static void testDodecahedron(size_t num_of_interior_points);
	static void test5Cell(size_t num_of_interior_points);
	static void test16Cell(size_t num_of_interior_points);
	static void testTesseract(size_t num_of_interior_points);
	static void test24Cell(size_t num_of_interior_points);
	static void testPenterakt(size_t num_of_interior_points);

private:
	static int signOfSemiSpace(const std::vector<MathVector>& points_of_hyperface, const MathVector& test_point);
	static void testPolyhedron(size_t num_of_interior_points, const std::vector<MathVector>& vertices, const std::vector<std::vector<MathVector>>& hyperfaces, void (*postprocessing_points)(std::vector<MathVector>& points));
	static void testRandomPointCloud(size_t num_of_points);

	static void setFilledPolyhedron(size_t num_of_interior_points, const std::vector<MathVector>& vertices, const std::vector<std::vector<MathVector>>& hyperfaces, std::vector<MathVector>& result);
	static void testNonSymplicialAlgorithm(size_t num_of_interior_points, const std::vector<MathVector>& vertices, const std::vector<std::vector<MathVector>>& hyperfaces);

	// -----
	static void rotate_translate(std::vector<MathVector>& points, const std::vector<double>& angles, const MathVector& offset);
	static void generate_rotation_matrix(double angle, size_t axis_1, size_t axis_2, size_t dimension, std::vector<std::vector<double>>& res_matrix);
	static void generate_regular_polygon_2D(size_t num_of_vertices, size_t num_of_interior_points, std::vector<MathVector>& vertices, std::vector<std::vector<MathVector>>& hyperfaces);
	// -----
};

