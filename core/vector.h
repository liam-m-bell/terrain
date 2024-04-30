#ifndef VECTOR_H_
#define VECTOR_H_

#include <math.h>

class Vector{
public:
	double x;
	double y;
	double z;

	Vector(double a, double b, double c){
		x = a;
		y = b;
		z = c;
	}

    Vector(double a, double b){
		x = a;
		y = b;
		z = 0.0;
	}

	Vector(){
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}

	double lengthSquared(){
	  return x * x + y * y + z * z ;
	}

	double length(){
	  return sqrt(lengthSquared());
	}

	Vector operator+(const Vector v){
		Vector result;
		result.x = this->x + v.x;
		result.y = this->y + v.y;
		result.z = this->z + v.z;
		return result;
	}

	Vector operator-(){
		Vector result;
		result.x = -this->x;
		result.y = -this->y;
		result.z = -this->z;
		return result;
	}

	Vector operator*(double scalar){
		Vector result;
		result.x = scalar * this->x;
		result.y = scalar * this->y;
		result.z = scalar * this->z;
		return result;
	}

	friend Vector operator-(Vector a, Vector b){
		Vector result;
		result.x = a.x - b.x;
		result.y = a.y - b.y;
		result.z = a.z - b.z;
		return result;
	}

	friend Vector operator*(double scalar, Vector v){
	  Vector result;
	  result.x = scalar * v.x;
	  result.y = scalar * v.y;
	  result.z = scalar * v.z;
	  return result;
	}
};

#endif
