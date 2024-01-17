#include "vector.h"

Vector vector3(float x, float y, float z){
    Vector v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

Vector vector2(float x, float y){
    return vector3(x, y, 0.0f);
}

float lenthSquared(Vector v){
    return(v.x * v.x + v.y * v.y + v.z * v.z);
}

float length(Vector v){
    return (float)sqrt(lenthSquared(v));
}

Vector normalise(Vector v){
    Vector n;
    float len = length(v);
    n.x = v.x / len;
    n.y = v.y / len;
    n.z = v.z / len;
}

float dot(Vector a, Vector b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector cross(Vector a, Vector b){
    Vector result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

Vector scaleVec(Vector v, float scalar){
    Vector result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    result.z = v.z * scalar;
    return result;
}

Vector addVec(Vector a, Vector b){
    Vector result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}
