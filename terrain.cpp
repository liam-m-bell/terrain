#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <cstring>

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
    float **upliftField = importImageAsHeightfield((char*)"uplift1.ppm", &upliftFieldSize, maxUplift);

    // Erosion
    int nodeCount = 40000;
    StreamGraph sg = StreamGraph(nodeCount, size, upliftField, upliftFieldSize);
    sg.initialise();

    std::cout << "Initialised\n";
    std::cout.flush();

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 250; i++){
        bool converged = sg.update();
        // if (converged){
        //     break;
        // }

        // float resolution = 64;
        // float standardDev = 180 / resolution;

        // float maxHeight;
        // float **heightfield = sg.createHightfield(resolution, standardDev, &maxHeight);
        // std::string filenameString = "steps/image";
        // filenameString.append(std::to_string(i));
        // filenameString.append(".ppm");
        // char filename[filenameString.length() + 1]; 
	    // strcpy(filename, filenameString.c_str()); 
        // outputHeightfieldAsImage(heightfield, size / resolution, maxHeight, filename);
        // freeHeightfield(heightfield, size / resolution);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Total" << (float)duration.count() / 1000000 << "\n";

    //Create mesh from graph and export mesh as OBJ file
    // Mesh *mesh = sg.createMesh();
    // exportMeshAsObj(mesh, "model.obj");
    // freeMesh(mesh);
    
    float resolution = 64;
    float standardDev = 180 / resolution;

    float maxHeight;
    float **heightfield = sg.createHightfield(resolution, standardDev, &maxHeight);
    Mesh *mesh = createMeshFromHeightfield(heightfield, size / resolution);
    
    exportMeshAsObj(mesh, (char*)"model.obj");
    freeMesh(mesh);

    outputHeightfieldAsImage(heightfield, size / resolution, maxHeight, (char*)"image.ppm");
    freeHeightfield(heightfield, size / resolution);

    freeHeightfield(upliftField, upliftFieldSize);
    return 0;
}

void generateImageFrame(char* heightfieldFilename, int terrainSize, float resolution, float standardDev){
    float maxHeight;
    float **heightfield = sg.createHightfield(resolution, standardDev, &maxHeight);
    std::string filenameString = heightfieldFilename;
    filenameString.append(std::to_string(i));
    filenameString.append(".ppm");
    char filename[filenameString.length() + 1]; 
    strcpy(filename, filenameString.c_str()); 
    outputHeightfieldAsImage(heightfield, terrainSize / resolution, maxHeight, filename);
    freeHeightfield(heightfield, terrainSize / resolution);
}


bool hasArg(int argc, char *argv[], char* argName){
    for (int i = 0; i < argc; i++){
        if (strcmp(argv[i], argName) == 0){
            return true;
        }  
    }
    return false;
}

char *getArg(int argc, char *argv[], char* argName){
    for (int i = 0; i < argc; i++){
        if ((strcmp(argv[i], argName) == 0) && (i + 1 < argc)){
            return argv[i+1];     
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    // Parse arguments
    if (argc > 24) {
        std::cout << "Too many arguments!";
        return 1;
    }

    // Parameters
    int terrainSize = getArg(argc, argv, "-terrainSize"); // Size of the terrain
    int nodeCount = getArg(argc, argv, "-nodeCount"); // Approximate number of points to sample in the terrain

    // Stream power equation
    float m = getArg(argc, argv, "-m"); // Drainage area constant
    float n = getArg(argc, argv, "-n"); // Slope constant
    float k = getArg(argc, argv, "-k"); // Erosion constant

    // Simulation iteration
    float timeStep = getArg(argc, argv, "-timeStep"); // Simulation time step
    int maxTimeSteps = getArg(argc, argv, "-maxTimeSteps"); //Maximum number of time steps to run before stopping
    float convergenceThreshold = getArg(argc, argv, "-convergenceThreshold"); // Value to determine if simulation can stop before max time steps

    // Uplift
    float maximumUplift = getArg(argc, argv, "-maximumUplift"); // Maximum value of the uplift to control height of mountains
    char *upliftFieldFilename = getArg(argc, argv, "-upliftFieldFilename"); // Filename of ppm image file which contains the uplift field

    // Thermal erosion
    float minimumTalusAngle = getArg(argc, argv, "-minimumTalusAngle"); // Minium thermal erosion talus angle
    float maximumTalusAngle = getArg(argc, argv, "-maximumTalusAngle"); // Maximum thermal erosion talus angle
    char *perlinNoiseDataFilename = getArg(argc, argv, "-perlinNoiseDataFilename"); //Filename of perlin noise seed data

    // Ouput mesh
    bool generateMesh = hasArg(argc, argv, "-generateMesh"); // Should tesselated mesh of terrain be generated
    char *meshFilename = getArg(argc, argv, "-meshFilename"); // Filename to write mesh to

    // Output heightfield
    bool generateHeightfield = hasArg(argc, argv, "-generateHeightfield"); // Should heightfield be generated
    char* heightfieldFilename = getArg(argc, argv, "-heightfieldFilename"); // Filename to write heightfield ppm image file to
    float heightfieldResolution = getArg(argc, argv, "-heightfieldResolution"); // Resolution of heightfield
    float heightfieldStandardDeviation = getArg(argc, argv, "-heightfieldStandardDeviation"); // Standard deviation of gaussian filter for heightfield generation
    bool generateHeightfieldMesh = hasArg(argc, argv, "-generateHeightfieldMesh");// Should heightfield mesh be generated
    char* heightfieldMeshFilename = getArg(argc, argv, "-heightfieldMeshFilename"); // Filename to write heightfield mesh file to
    bool generateImageSequence = hasArg(argc, argv, "-generateImageSequence"); //Should heightfield image sequence be generated
    int imageSequenceInterval = getArg(argc, argv, "-imageSequenceInterval"); // Interval of frames of image sequence
    
    // Main program
    srand(time(NULL));
    loadNoisePermutation(perlinNoiseDataFilename);

    // Import uplift
    int upliftFieldSize;
    float **upliftField = importImageAsHeightfield(upliftFieldFilename, &upliftFieldSize, maximumUplift);

    // Initialise
    StreamGraph sg = StreamGraph(nodeCount, terrainSize, upliftField, upliftFieldSize);
    sg.initialise();
    std::cout << "Initialised\n";
    std::cout.flush();

    // Main simulation loop
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < maxTimeSteps; i++){
        // Update terrain
        bool converged = sg.update();

        // if (converged){
        //     break;
        // }

        if (generateImageSequence && i % imageSequenceInterval == 0){
            generateImageFrame(heightfieldFilename, terrainSize, heightfieldResolution, heightfieldStandardDeviation)
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time elapsed: " << (float)duration.count() / 1000000 << "s\n";

    freeHeightfield(upliftField, upliftFieldSize);

    if (generateMesh){
        //Create mesh from graph and export mesh as OBJ file
        Mesh *mesh = sg.createMesh();
        exportMeshAsObj(mesh, meshFilename);
        freeMesh(mesh);
    }

    if (generateHeightfield){
        // Generate heightfield and output as image
        float maxHeight;
        float **heightfield = sg.createHightfield(heightfieldResolution, heightfieldStandardDeviation, &maxHeight);
        outputHeightfieldAsImage(heightfield, terrainSize / heightfieldResolution, maxHeight, heightfieldFilename);

        if (generateHeightfieldMesh){
            // Generate mesh from heightfield
            Mesh *mesh = createMeshFromHeightfield(heightfield, terrainSize / heightfieldResolution);
            exportMeshAsObj(mesh, heightfieldMeshFilename);
            freeMesh(mesh);
        }

        freeHeightfield(heightfield, terrainSize / heightfieldResolution);
    }

    return 0;
}