#ifndef NOISE_H_
#define NOISE_H_

#include "vector.h"

void loadNoisePermutation(char* fileName);
double noise3D(double x, double y, double z);
double noise2D(double x, double y);

double perlinNoise(Vector p, int octaves, double lacunarity, double persistence, double scale);
double billowNoise(Vector p, int octaves, double lacunarity, double persistence, double scale);
double ridgeNoise(Vector p, int octaves, double lacunarity, double persistence, double scale);

double warpedNoise(Vector warp, double warpScale, Vector p, int octaves, double lacunarity, double persistence, double scale);

double randRange(double min, double max);

#endif