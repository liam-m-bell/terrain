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

void generateHeightfieldFromNoise(float** heightmap, int size) {
    for (int z = 0; z < size; z++) {
        for (int x = 0; x < size; x++) {
            float elevation = ridgeNoise(x * 10 / (float)size, z * 10 / (float)size, 6, 0.5f, 0.5f);
            
            heightmap[z][x] = elevation * 10;
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
    generateHeightfieldFromNoise(heightfield, size);
    //diamondSquare(heightfield, size, 0.45);
    
	//printf("Perlin Noise for (%0.2f,%0.2f,%0.2f) is %.17lf",3.14f ,42.0f, 7.0f, noise2D(3.14, 42));

    // Create mesh from heightfield
    Mesh *mesh = createMeshFromHeightfield(heightfield, size);

    // Export mesh as OBJ file
    const char* objFilename = "output.obj";
    exportMeshAsObj(mesh, objFilename);

    freeMesh(mesh);
    freeHeightfield(heightfield, size);

    return 0;
}
