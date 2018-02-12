#pragma once

#ifdef MATHVECTOR_EXPORTS
#define MATHVECTOR_API __declspec(dllexport) 
#else
#define MATHVECTOR_API __declspec(dllimport) 
#endif

#include <vector>

class MathVector
{
public:
	MATHVECTOR_API MathVector();
	MATHVECTOR_API MathVector(const std::vector<double>&);
	MATHVECTOR_API MathVector(size_t);
	MATHVECTOR_API ~MathVector();

	MATHVECTOR_API size_t getDimension() const;
	MATHVECTOR_API void normalize();
	MATHVECTOR_API static MathVector crossProduct(const std::vector<MathVector>&);

	MATHVECTOR_API friend MathVector    operator+(const MathVector&, const MathVector&);
	MATHVECTOR_API friend MathVector    operator-(const MathVector&, const MathVector&);
	MATHVECTOR_API friend MathVector    operator*(double, const MathVector&);
	MATHVECTOR_API friend double        operator*(const MathVector&, const MathVector&);
	MATHVECTOR_API friend MathVector    operator*(const std::vector<std::vector<double>>&, const MathVector&);

	MATHVECTOR_API friend std::ostream& operator<<(std::ostream&, const MathVector&);
	MATHVECTOR_API friend std::istream& operator>>(std::istream&, MathVector&);

	MATHVECTOR_API double& operator[](size_t);
	MATHVECTOR_API double  operator[](size_t) const;

private:
	std::vector<double> math_vector;

	static double determinant(std::vector<std::vector<double>>&);
};