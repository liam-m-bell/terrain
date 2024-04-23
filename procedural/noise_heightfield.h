#ifndef NOISE_HEIGHTFIELD_H_
#define NOISE_HEIGHTFIELD_H_

void generateHeightfieldFromNoise(double** heightfield, int size, int octaves, double lacunarity, double persistence, double scale, double height);

#endif