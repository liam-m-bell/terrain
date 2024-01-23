#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "core/heightfield.h"
#include "core/mesh.h"
#include "core/noise.h"
#include "procedural/diamond_square.h"
#include "procedural/noise_heightfield.h"
#include "simulation/musgrave_erosion.h"

int main(){
    srand(time(NULL));
    loadNoisePermutation((char*)"perlin_data.txt");

    const int n = 8;
    const int size = pow(2, n) + 1;

    // Create heightfield
    float **heightfield = createHeightfield(size);

    // Generate heightfield
    generateHeightfieldFromNoise(heightfield, size, 5, 2.0f, 0.5f, 4.0f, size / 5.0f);

    // Simulation
    musgraveErosion(heightfield, size, 10, 0.5f, 0.01f, 0.3f, 10, 0.005f, 0.4f, 0.05f);

    // Output heightfield as image
    outputHeightfieldAsImage(heightfield, size, size / 5.0f, (char*)"image.ppm");

    // Create mesh from heightfield and export as OBJ file
    Mesh *mesh = createMeshFromHeightfield(heightfield, size);
    exportMeshAsObj(mesh, "model.obj");
    freeMesh(mesh);

    freeHeightfield(heightfield, size);

    return 0;
}
