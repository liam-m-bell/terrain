#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "mesh.h"
#include "diamond_square.h"
#include "noise.h"

// Allocates memory for a heightfield
float **createHeightfield(const int size){
    float **a = (float**)malloc(size * sizeof(float*));
    for (int i = 0; i < size; i++){
        a[i] = (float*)malloc(size * sizeof(float));
    }

    return a;
}

// Deallocates memory for a heightfield
void freeHeightfield(float **a, const int size){
    for (int i = 0; i < size; i++) {
        free(a[i]);
    }
    free(a);
}

void generateHeightfieldFromNoise(float** heightfield, int size, int octaves, float lacunarity, float persistence, float scale, float height){
    float min = 0.0f;
    float max = 0.0f;

    for (int z = 0; z < size; z++){
        for (int x = 0; x < size; x++){
            float elevation = perlinNoise(x / (float)size, z / (float)size, octaves, lacunarity, persistence, scale);
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

    const int n = 11;
    const int size = pow(2, n) + 1;

    // Create heightfield
    float **heightfield = createHeightfield(size);

    // Generate heightfield
    generateHeightfieldFromNoise(heightfield, size, 9, 2.0f, 0.5f, 4.0f, size / 5.0f);
    //diamondSquare(heightfield, size, 0.45);

    // Create mesh from heightfield
    Mesh *mesh = createMeshFromHeightfield(heightfield, size);

    // Export mesh as OBJ file
    const char* objFilename = "output.obj";
    exportMeshAsObj(mesh, objFilename);

    freeMesh(mesh);
    freeHeightfield(heightfield, size);

    return 0;
}
