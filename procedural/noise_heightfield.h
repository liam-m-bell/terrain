#ifndef NOISE_HEIGHTFIELD_H_
#define NOISE_HEIGHTFIELD_H_

void generateHeightfieldFromNoise(float** heightfield, int size, int octaves, float lacunarity, float persistence, float scale, float height);

#endif