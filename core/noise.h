#ifndef NOISE_H_
#define NOISE_H_

#include "vector.h"

void loadNoisePermutation(char* fileName);
double noise3D(double x, double y, double z);
double noise2D(double x, double y);

float perlinNoise(Vector p, int octaves, float lacunarity, float persistence, float scale);
float billowNoise(Vector p, int octaves, float lacunarity, float persistence, float scale);
float ridgeNoise(Vector p, int octaves, float lacunarity, float persistence, float scale);

float warpedNoise(Vector warp, float warpScale, Vector p, int octaves, float lacunarity, float persistence, float scale);

float randRange(float min, float max);

#endif