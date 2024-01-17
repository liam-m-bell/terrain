#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "heightfield.h"
#include "mesh.h"
#include "diamond_square.h"
#include "noise.h"

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

int main(){
    srand(time(NULL));
    loadNoisePermutation("perlin_data.txt");

    const int n = 8;
    const int size = pow(2, n) + 1;

    // Create heightfield
    float **heightfield = createHeightfield(size);

    // Generate heightfield
    generateHeightfieldFromNoise(heightfield, size, 9, 2.0f, 0.5f, 4.0f, size / 5.0f);

    // Output heightfield as image
    outputHeightfieldAsImage(heightfield, size, size / 5.0f, "image.ppm");

    // Create mesh from heightfield and export as OBJ file
    Mesh *mesh = createMeshFromHeightfield(heightfield, size);
    exportMeshAsObj(mesh, "model.obj");
    freeMesh(mesh);

    freeHeightfield(heightfield, size);

    return 0;
}
