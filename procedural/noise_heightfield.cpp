#include <stdlib.h>

#include "../core/vector.h"
#include "../core/noise.h"

// Noise functions
double perlinNoise(Vector p, int octaves, double lacunarity, double persistence, double scale){
	double sum = 0.0f;
	double frequency = scale;
	double amplitude = 1.0f;

	for(int i = 0; i < octaves; i++) {
		double n = noise2D(p.x * frequency, p.y * frequency);
		sum += n * amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	} 

	return sum;
}

double billowNoise(Vector p, int octaves, double lacunarity, double gain, double scale){
	return fabs(perlinNoise(p, octaves, lacunarity, gain, scale));
}

double ridgeNoise(Vector p, int octaves, double lacunarity, double gain, double scale){
	return 1.0f - billowNoise(p, octaves, lacunarity, gain, scale);
}

double warpedNoise(Vector warp, double warpScale, Vector p, int octaves, double lacunarity, double persistence, double scale){
    Vector offset = Vector(perlinNoise(p, octaves, lacunarity, persistence, scale), 
            perlinNoise(p + warp, octaves, lacunarity, persistence, scale));

    return perlinNoise(p + offset * warpScale, octaves, lacunarity, persistence, scale);
}

void generateHeightfieldFromNoise(double** heightfield, int size, int octaves, double lacunarity, double persistence, double scale, double height){
    double min = 0.0f;
    double max = 0.0f;

    for (int z = 0; z < size; z++){
        for (int x = 0; x < size; x++){
            //double elevation = perlinNoise(Vector(x / (double)size, z / (double)size), octaves, lacunarity, persistence, scale);
            double elevation = warpedNoise(Vector(5.2f, 1.3f), 0.0f, Vector(x / (double)size, z / (double)size), octaves, lacunarity, persistence, scale);
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
