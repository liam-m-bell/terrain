#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <chrono>

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

    const int n = 15;
    const int size = pow(2, n) + 1;

    // Import uplift
    float maxUplift = 5.0f * pow(10.0f, -4);
    int upliftFieldSize;
    float **upliftField = importImageAsHeightfield((char*)"uplift.ppm", &upliftFieldSize, maxUplift);

    // Erosion
    StreamGraph sg = StreamGraph(8000, size, upliftField, upliftFieldSize);
    sg.initialise();

    std::cout << "Initialised";
    std::cout.flush();

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; i++){
        bool converged = sg.update();
        if (converged){
            break;
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Total" << (float)duration.count() / 1000000 << "\n";

    // Create mesh from graph and export mesh as OBJ file
    Mesh *mesh = sg.createMesh();
    exportMeshAsObj(mesh, "model.obj");
    freeMesh(mesh);

    outputHeightfieldAsImage(upliftField, upliftFieldSize, maxUplift, (char*)"uplift-imported.ppm");
    freeHeightfield(upliftField, upliftFieldSize);
    return 0;
}
