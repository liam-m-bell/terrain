/****************************************************************************
*
* krt - Ken's Raytracer - Coursework Edition. (C) Copyright 1993-2022.
*
* I've put a lot of time and effort into this code. For the last decade
* it's been used to introduce hundreds of students at multiple universities
* to raytracing. It forms the basis of your coursework but you are free
* to continue using/developing forever more. However, I ask that you don't
* share the code or your derivitive versions publicly. In order to continue
* to be used for coursework and in particular assessment it's important that
* versions containing solutions are not searchable on the web or easy to
* download.
*
* If you want to show off your programming ability, instead of releasing
* the code, consider generating an incredible image and explaining how you
* produced it.
*/

// A three element vector class with lots of operators and common functions

#ifndef VECTOR_H_
#define VECTOR_H_

#include <math.h>

class Vector {
public:
	float x;
	float y;
	float z;

	Vector(float px, float py, float pz)
	{
		x = px;
		y = py;
		z = pz;
	}

    Vector(float px, float py)
	{
		x = px;
		y = py;
	}

	Vector()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	void normalise()
	{
		float len = (float)sqrt((double)(x*x + y*y + z*z));
		x = x / len;
		y = y / len;
		z = z / len;
	}

	float len_sqr()
	{
	  return(x*x + y*y + z*z);
	}

	float length()
	{
	  return(float)sqrt((double)(x*x + y*y + z*z));
	}

	float dot(Vector other)
	{
		return x*other.x + y*other.y + z*other.z;
	}


	void reflection(Vector initial, Vector &reflect)
	{
		float d;

		d = dot(initial);
		d = d * 2.0f;

		reflect.x = initial.x - d * x;
		reflect.y = initial.y - d * y;
		reflect.z = initial.z - d * z;
	}

	void negate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	void cross(Vector &other, Vector &result)
	{
	  result.x = y*other.z - z*other.y;
	  result.y = z*other.x - x*other.z;
	  result.z = x*other.y - y*other.x;
	}

	void cross(Vector &other)
	{
	  Vector result;
	  result.x = y*other.z - z*other.y;
	  result.y = z*other.x - x*other.z;
	  result.z = x*other.y - y*other.x;
	  x = result.x;
	  y = result.y;
	  z = result.z;
	}

	Vector& operator=(const Vector& other)
	{
	  this->x = other.x;
	  this->y = other.y;
	  this->z = other.z;
	  return *this;
	}


	Vector operator*(const Vector& b)
	{
		Vector r;
		r.x = this->x * b.x;
		r.y = this->y * b.y;
		r.z = this->z * b.z;

		return r;
	}

	friend Vector operator-(const Vector& a, const Vector& b)
	{
		Vector r;
		r.x = a.x - b.x;
		r.y = a.y - b.y;
		r.z = a.z - b.z;

		return r;
	}

	Vector operator+(const Vector& b)
	{
		Vector r;
		r.x = this->x + b.x;
		r.y = this->y + b.y;
		r.z = this->z + b.z;

		return r;
	}


	Vector operator*(float& b)
	{
		Vector r;
		r.x = this->x * b;
		r.y = this->y * b;
		r.z = this->z * b;

		return r;
	}

	friend Vector operator*(const float a, const Vector &b)
	{
	  Vector t;
	  t.x = a * b.x;
	  t.y = a * b.y;
	  t.z = a * b.z;
	  return t;
	}

	Vector operator-()
	{
	  return Vector(-this->x, -this->y, -this->z);
	}
};

#endif
