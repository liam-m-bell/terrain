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

void generateImageFrame(StreamGraph *sg, int iteration, char* heightfieldFilename, int terrainSize, float resolution, float standardDev){
    float maxHeight;
    float **heightfield = sg->createHightfield(resolution, standardDev, &maxHeight);
    std::string filenameString = heightfieldFilename;
    filenameString.append(std::to_string(iteration));
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
    if (argc > 45) {
        std::cout << "Too many arguments!";
        return 1;
    }

    // Parameters
    int terrainSize = atoi(getArg(argc, argv, (char*)"-terrainSize")); // Size of the terrain
    int nodeCount = atoi(getArg(argc, argv, (char*)"-nodeCount")); // Approximate number of points to sample in the terrain

    // Stream power equation
    float m = atof(getArg(argc, argv, (char*)"-m")); // Drainage area constant
    float n = atof(getArg(argc, argv, (char*)"-n")); // Slope constant
    float k = atof(getArg(argc, argv, (char*)"-k")); // Erosion constant

    // Simulation iteration
    float timeStep = atof(getArg(argc, argv, (char*)"-timeStep")); // Simulation time step
    int maxTimeSteps = atoi(getArg(argc, argv, (char*)"-maxTimeSteps")); //Maximum number of time steps to run before stopping
    float convergenceThreshold = atof(getArg(argc, argv, (char*)"-convergenceThreshold")); // Value to determine if simulation can stop before max time steps

    // Uplift
    float maximumUplift = atof(getArg(argc, argv, (char*)"-maximumUplift")); // Maximum value of the uplift to control height of mountains
    char *upliftFieldFilename = getArg(argc, argv, (char*)"-upliftFieldFilename"); // Filename of ppm image file which contains the uplift field

    // Thermal erosion
    float minimumTalusAngle = atof(getArg(argc, argv, (char*)"-minimumTalusAngle")); // Minium thermal erosion talus angle
    float maximumTalusAngle = atof(getArg(argc, argv, (char*)"-maximumTalusAngle")); // Maximum thermal erosion talus angle
    char *perlinNoiseDataFilename = getArg(argc, argv, (char*)"-perlinNoiseDataFilename"); //Filename of perlin noise seed data

    // Rainfall
    bool variableRainfall = hasArg(argc, argv, (char*)"-variableRainfall");
    float maximumRainfall = atof(getArg(argc, argv, (char*)"-maximumRainfall")); // Maximum value of the rainfall
    char *rainfallFieldFilename = getArg(argc, argv, (char*)"-rainfallFieldFilename"); // Filename of ppm image file which contains the rainfall field

    // Ouput mesh
    bool generateMesh = hasArg(argc, argv, (char*)"-generateMesh"); // Should tesselated mesh of terrain be generated
    char *meshFilename = getArg(argc, argv, (char*)"-meshFilename"); // Filename to write mesh to

    // Output heightfield
    bool generateHeightfield = hasArg(argc, argv, (char*)"-generateHeightfield"); // Should heightfield be generated
    char* heightfieldFilename = getArg(argc, argv, (char*)"-heightfieldFilename"); // Filename to write heightfield ppm image file to
    float heightfieldResolution = atof(getArg(argc, argv, (char*)"-heightfieldResolution")); // Resolution of heightfield
    float heightfieldStandardDeviation = atof(getArg(argc, argv, (char*)"-heightfieldStandardDeviation")); // Standard deviation of gaussian filter for heightfield generation
    bool generateHeightfieldMesh = hasArg(argc, argv, (char*)"-generateHeightfieldMesh");// Should heightfield mesh be generated
    char* heightfieldMeshFilename = getArg(argc, argv, (char*)"-heightfieldMeshFilename"); // Filename to write heightfield mesh file to
    bool generateImageSequence = hasArg(argc, argv, (char*)"-generateImageSequence"); //Should heightfield image sequence be generated
    int imageSequenceInterval = atoi(getArg(argc, argv, (char*)"-imageSequenceInterval")); // Interval of frames of image sequence
    
    // Main program
    srand(time(NULL));
    loadNoisePermutation(perlinNoiseDataFilename);

    // Import uplift
    int upliftFieldSize;
    float **upliftField = importImageAsHeightfield(upliftFieldFilename, &upliftFieldSize, maximumUplift);

    // Import rainfall
     int rainfallFieldSize;
    float **rainfallField = importImageAsHeightfield(rainfallFieldFilename, &rainfallFieldSize, maximumRainfall);

    // Initialise
    StreamGraph sg = StreamGraph(terrainSize, timeStep, upliftField, upliftFieldSize, variableRainfall, rainfallField, rainfallFieldSize);
    sg.initialise(nodeCount, m, n, k, convergenceThreshold, minimumTalusAngle, maximumTalusAngle);

    // Main simulation loop
    std::cout << "Running Simulation";
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < maxTimeSteps; i++){
        // Update terrain(char*)
        bool converged = sg.update();
        
        std::cout << ".";

        if (converged){
            std::cout << "Model converged in " << i << "time steps\n";
            break;
        }

        if (generateImageSequence && i % imageSequenceInterval == 0){
            generateImageFrame(&sg, i, heightfieldFilename, terrainSize, heightfieldResolution, heightfieldStandardDeviation);
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "\nTime elapsed: " << (float)duration.count() / 1000000 << "s\n";

    freeHeightfield(upliftField, upliftFieldSize);

    if (variableRainfall){
        freeHeightfield(rainfallField, rainfallFieldSize);
    }

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