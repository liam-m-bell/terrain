#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <cstring>
#include <iostream>
#include <fstream>

#include "core/heightfield.h"
#include "core/mesh.h"
#include "core/noise.h"

#include "streamPower/stream_graph.h"

// Saves a frame of the heightfield
void generateImageFrame(StreamGraph *sg, int iteration, char* heightfieldFilename, int terrainSize, double resolution, double standardDev){
    double maxHeight;
    double **heightfield = sg->createHightfield(resolution, standardDev, &maxHeight);
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
    if (argc > 46) {
        std::cout << "Too many arguments!";
        return 1;
    }

    // Perlin noise
    char *perlinNoiseDataFilename = getArg(argc, argv, (char*)"-perlinNoiseDataFilename"); 

    // Parameters

    // Size of the terrain
    int terrainSize = atoi(getArg(argc, argv, (char*)"-terrainSize"));
    // Approximate number of points to sample in the terrain
    int nodeCount = atoi(getArg(argc, argv, (char*)"-nodeCount"));

    // Stream power equation
    // Drainage area constant
    double m = atof(getArg(argc, argv, (char*)"-m"));
    // Slope constant
    double n = atof(getArg(argc, argv, (char*)"-n"));
    // Erosion constant
    double k = atof(getArg(argc, argv, (char*)"-k"));

    // Simulation iteration
    // Simulation time step
    double timeStep = atof(getArg(argc, argv, (char*)"-timeStep"));
    //Maximum number of time steps to run before stopping
    int maxTimeSteps = atoi(getArg(argc, argv, (char*)"-maxTimeSteps"));
    // Value to determine if simulation can stop before max time steps
    double convergenceThreshold = atof(getArg(argc, argv, (char*)"-convergenceThreshold"));

    // Uplift
    // Maximum value of the uplift to control height of mountains
    double maximumUplift = atof(getArg(argc, argv, (char*)"-maximumUplift"));
    // Filename of ppm image file which contains the uplift field
    char *upliftFieldFilename = getArg(argc, argv, (char*)"-upliftFieldFilename");

    // Thermal erosion
    // Maximum thermal erosion talus angle
    double talusAngle = atof(getArg(argc, argv, (char*)"-talusAngle")); 

    // Rainfall
    bool variableRainfall = hasArg(argc, argv, (char*)"-variableRainfall");
    // Maximum value of the rainfall
    double maximumRainfall = atof(getArg(argc, argv, (char*)"-maximumRainfall"));
    // Filename of ppm image file which contains the rainfall field
    char *rainfallFieldFilename = getArg(argc, argv, (char*)"-rainfallFieldFilename"); 

    // Ouput mesh
    // Should tesselated mesh of terrain be generated
    bool generateMesh = hasArg(argc, argv, (char*)"-generateMesh");
    // Filename to write mesh to
    char *meshFilename = getArg(argc, argv, (char*)"-meshFilename");

    // Output heightfield
    // Should heightfield be generated
    bool generateHeightfield = hasArg(argc, argv, (char*)"-generateHeightfield");
    // Filename to write heightfield ppm image file to
    char* heightfieldFilename = getArg(argc, argv, (char*)"-heightfieldFilename");
    // Size of heightfield
    int heightfieldSize = atoi(getArg(argc, argv, (char*)"-heightfieldSize"));
    // Standard deviation of gaussian filter for heightfield generation
    double heightfieldStandardDeviation = atof(getArg(argc, argv, (char*)"-heightfieldStandardDeviation"));
    // Should heightfield mesh be generated
    bool generateHeightfieldMesh = hasArg(argc, argv, (char*)"-generateHeightfieldMesh");
    // Filename to write heightfield mesh file to
    char* heightfieldMeshFilename = getArg(argc, argv, (char*)"-heightfieldMeshFilename");
    //Should heightfield image sequence be generated
    bool generateImageSequence = hasArg(argc, argv, (char*)"-generateImageSequence");
    // Interval of frames of image sequence
    int imageSequenceInterval = atoi(getArg(argc, argv, (char*)"-imageSequenceInterval"));
    
    // Main program
    srand(time(NULL));
    loadNoisePermutation(perlinNoiseDataFilename);

    double heightfieldResolution = (float)terrainSize / (float)heightfieldSize;

    std::ofstream resultFile;
    resultFile.open("results.txt");
    resultFile << "Arguments:\n";
    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-'){
            resultFile << "\n";
        }
        else{
            resultFile << " ";
        }
        resultFile << argv[i];
    }

    // Import uplift
    int upliftFieldSize;
    double **upliftField = importImageAsHeightfield(upliftFieldFilename, &upliftFieldSize, maximumUplift);

    // Import rainfall
    int rainfallFieldSize;
    double **rainfallField = importImageAsHeightfield(rainfallFieldFilename, &rainfallFieldSize, maximumRainfall);

    // Initialise
    StreamGraph sg = StreamGraph(terrainSize, timeStep, upliftField, upliftFieldSize, maximumUplift, variableRainfall, rainfallField, rainfallFieldSize);
    sg.initialise(nodeCount, m, n, k, convergenceThreshold, talusAngle);

    std::cout << "Initialised stream graph\n";
    std::cout.flush();
    resultFile << "\n\nNode count: " << sg.nodes.size();

    std::ofstream timesFile;
    timesFile.open("times.csv");
    timesFile << "Iteration, LakeCount, Duration, MaxHeight\n";

    // Main simulation loop
    std::cout << "Running Simulation";
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < maxTimeSteps; i++){
        // Update terrain
        bool converged = sg.update();

        std::cout << ".";
        std::cout.flush();

        if (converged){
            resultFile << "\nModel converged in " << i << " time steps";
            break;
        }

        if (generateImageSequence && i % imageSequenceInterval == 0){
            generateImageFrame(&sg, i, heightfieldFilename, terrainSize, heightfieldResolution, heightfieldStandardDeviation);
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    // Write info to file:
    resultFile << "\nTime elapsed: " << (double)duration.count() / 1000000 << "s\n";
    resultFile << "Maximum height: " << sg.getMaxHeight() << "m\n";
    resultFile.close();

    freeHeightfield(upliftField, upliftFieldSize);

    if (rainfallField != 0){
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
        double maxHeight;
        double **heightfield = sg.createHightfield(heightfieldResolution, heightfieldStandardDeviation, &maxHeight);
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