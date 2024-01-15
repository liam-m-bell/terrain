#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "mesh.h"
#include "diamond_square.h"

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

int main(){
    srand(time(NULL));

    const int n = 7;
    const int size = pow(2, n) + 1;

    // Create heightfield
    float **heightfield = createHeightfield(size);

    // Generate heightfield
    diamondSquare(heightfield, size, 0.45);

    // Create mesh from heightfield
    Mesh *mesh = createMeshFromHeightfield(heightfield, size);

    // Export mesh as OBJ file
    const char* objFilename = "output.obj";
    exportMeshAsObj(mesh, objFilename);

    free(mesh);
    freeHeightfield(heightfield, size);

    return 0;
}
