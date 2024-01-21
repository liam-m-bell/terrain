#include "../core/vector.h"
#include "../core/noise.h"

void generateHeightfieldFromNoise(float** heightfield, int size, int octaves, float lacunarity, float persistence, float scale, float height){
    float min = 0.0f;
    float max = 0.0f;

    for (int z = 0; z < size; z++){
        for (int x = 0; x < size; x++){
            //float elevation = perlinNoise(vector2(x / (float)size, z / (float)size), octaves, lacunarity, persistence, scale);
            float elevation = warpedNoise(vector2(5.2f, 1.3f), 0.0f, vector2(x / (float)size, z / (float)size), octaves, lacunarity, persistence, scale);
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
