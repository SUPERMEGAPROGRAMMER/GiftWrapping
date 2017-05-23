#pragma once

#include "GiftWrapping.h"
#include "MathVector.h"

class GiftWrappingTest
{
public:
	GiftWrappingTest();
	~GiftWrappingTest();

	static void testPyramid(size_t num_of_interior_points);
	static void testOctahedron(size_t num_of_interior_points);
	static void testIcosahedron(size_t num_of_interior_points);
	static void testCube(size_t num_of_interior_points);
	static void test5Cell(size_t num_of_interior_points);
	static void test16Cell(size_t num_of_interior_points);
	static void testTesseract(size_t num_of_interior_points);

private:
	static int signOfSemiSpace(const std::vector<MathVector>& points_of_hyperface, const MathVector& test_point);
	static void testPolyhedron(size_t num_of_interior_points, const std::vector<MathVector>& vertices, const std::vector<std::vector<MathVector>>& hyperfaces);
	static void testRandomPointCloud(size_t num_of_points);
};

