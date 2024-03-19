#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>

#include "core/heightfield.h"
#include "core/mesh.h"
#include "core/noise.h"
#include "procedural/diamond_square.h"
#include "procedural/noise_heightfield.h"
#include "simulation/musgrave_erosion.h"

#include "streamPower/stream_graph.h"

int main(){
    srand(time(NULL));
    loadNoisePermutation((char*)"perlin_data.txt");

    // // Create heightfield
    const int n = 15;
    const int size = pow(2, n) + 1;
    float **heightfield = createHeightfield(size);

    // Import uplift
    float **upliftField;
    float maxUplift = 5.0f * pow(10.0f, -4);
    int upliftFieldSize = importImageAsHeightfield((char*)"uplift.ppm", &upliftField, maxUplift);

    // Erosion
    StreamGraph sg = StreamGraph(10000, size, upliftField, upliftFieldSize);
    sg.initialise();

    std::cout << "Initialised";

    for (int i = 0; i < 100; i++){
        sg.update();
    }

    Mesh *mesh = sg.createMesh();

    // Create mesh from heightfield and export mesh as OBJ file

    exportMeshAsObj(mesh, "model.obj");
    freeMesh(mesh);

    freeHeightfield(heightfield, size);

    return 0;
}
