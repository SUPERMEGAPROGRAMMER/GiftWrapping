#include "stdafx.h"
#include "GiftWrappingTest.h"

int main()
{
	//GiftWrappingTest::testRegularPolygon2D(2, 100);
	//GiftWrappingTest::testRegularPolygon2D(3, 100);
	//GiftWrappingTest::testRegularPolygon2D(4, 100);


	GiftWrappingTest::testPyramid(100);	
	GiftWrappingTest::testOctahedron(100);
	GiftWrappingTest::testIcosahedron(100);
	GiftWrappingTest::testCube(0);
	GiftWrappingTest::testDodecahedron(100);
	GiftWrappingTest::test5Cell(100);
	GiftWrappingTest::test16Cell(100);
	GiftWrappingTest::testTesseract(0);
	GiftWrappingTest::test24Cell(10);

	system("pause");
	return 0;
}