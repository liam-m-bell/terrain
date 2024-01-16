#ifndef VECTOR_H_
#define VECTOR_H_

#include <math.h>

// Structure to represent a 3D vector
typedef struct{
    float x, y, z;
} Vector;

Vector vector3(float x, float y, float z);
Vector vector2(float x, float y);

float lenthSquared(Vector v);
float length(Vector v);
Vector normalise(Vector v);
float dot(Vector a, Vector b);
Vector cross(Vector a, Vector b);
Vector scale(Vector v, float scalar);
Vector add(Vector a, Vector b);

#endif