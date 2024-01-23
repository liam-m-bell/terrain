#include <stdlib.h>

#include "../core/vector.h"
#include "../core/noise.h"

// Noise functions
float perlinNoise(Vector p, int octaves, float lacunarity, float persistence, float scale){
	float sum = 0.0f;
	float frequency = scale;
	float amplitude = 1.0f;

	for(int i = 0; i < octaves; i++) {
		float n = noise2D(p.x * frequency, p.y * frequency);
		sum += n * amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	} 

	return sum;
}

float billowNoise(Vector p, int octaves, float lacunarity, float gain, float scale){
	return fabs(perlinNoise(p, octaves, lacunarity, gain, scale));
}

float ridgeNoise(Vector p, int octaves, float lacunarity, float gain, float scale){
	return 1.0f - billowNoise(p, octaves, lacunarity, gain, scale);
}

float randRange(float min, float max){
    return min + (max - min) * ((float)rand() / RAND_MAX);
}

float warpedNoise(Vector warp, float warpScale, Vector p, int octaves, float lacunarity, float persistence, float scale){
    Vector offset = Vector(perlinNoise(p, octaves, lacunarity, persistence, scale), 
            perlinNoise(p + warp, octaves, lacunarity, persistence, scale));

    return perlinNoise(p + offset * warpScale, octaves, lacunarity, persistence, scale);
}

void generateHeightfieldFromNoise(float** heightfield, int size, int octaves, float lacunarity, float persistence, float scale, float height){
    float min = 0.0f;
    float max = 0.0f;

    for (int z = 0; z < size; z++){
        for (int x = 0; x < size; x++){
            //float elevation = perlinNoise(Vector(x / (float)size, z / (float)size), octaves, lacunarity, persistence, scale);
            float elevation = warpedNoise(Vector(5.2f, 1.3f), 0.0f, Vector(x / (float)size, z / (float)size), octaves, lacunarity, persistence, scale);
            heightfield[z][x] = elevation;

            if (elevation < min){
                min = elevation;
            }
            
            if (elevation > max){
                max = elevation;
            }
        }
    }

    // Normalise heightfield to height
    if (max != min){
        for (int z = 0; z < size; z++){
            for (int x = 0; x < size; x++){
                heightfield[z][x] = (heightfield[z][x] - min) / (max - min) * height;
            }
        }
    }
}
