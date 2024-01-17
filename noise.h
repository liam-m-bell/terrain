#ifndef NOISE_H_
#define NOISE_H_

void loadNoisePermutation(char* fileName);
double noise3D(double x, double y, double z);
double noise2D(double x, double y);

float perlinNoise(float x, float y, int octaves, float lacunarity, float gain, float scale);
float billowNoise(float x, float y, int octaves, float lacunarity, float gain, float scale);
float ridgeNoise(float x, float y, int octaves, float lacunarity, float gain, float scale);

#endif