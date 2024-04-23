#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

// Allocates memory for a heightfield
double **createHeightfield(const int size){
    double **a = (double**)malloc(size * sizeof(double*));
    for (int i = 0; i < size; i++){
        a[i] = (double*)calloc(size, sizeof(double));
    }

    return a;
}

// Deallocates memory for a heightfield
void freeHeightfield(double **a, const int size){
    for (int i = 0; i < size; i++){
        free(a[i]);
    }
    free(a);
}

// Outputs a heightfield as a ppm image file to a specified file path
void outputHeightfieldAsImage(double **a, const int size, const double maxHeight, char *filename){
    // Adapted from https://rosettacode.org/wiki/Bitmap/Write_a_PPM_file
    FILE *imageFile = fopen(filename, "wb");
    fprintf(imageFile, "P6\n%d %d\n255\n", size, size);

    for (int z = 0; z < size; z++){
        for (int x = 0; x < size; x++){
            unsigned char color[3];

            // Normalise colour
            unsigned char value = (char)(a[z][x] * 255.0f / maxHeight);

            // Write colour to r,g and b channels.
            color[0] = value;
            color[1] = value;
            color[2] = value;
            fwrite(color, 1, 3, imageFile);
        }
    }

    fclose(imageFile);
}

// Imports a heightfield as a ppm image file
double **importImageAsHeightfield(char *filename, int *terrainSize, const double maxHeight){
    std::ifstream imageFile(filename);
    
    std::string lineString;

    // Check for standard header of file
    std::getline(imageFile, lineString);
    if (lineString[0] != 'P'){ 
        return 0;
    }

    // Ignore comments
    std::getline(imageFile, lineString);
    while (lineString[0] == '#') {
        std::getline(imageFile, lineString);
    }

    // Get the dimensions of the iamge
    std::istringstream dimensions(lineString);
    int width, height;
    dimensions >> width;
    dimensions >> height;
    if (width != height){
        return 0;
    }

    int size = width;
    double **heightfield = createHeightfield(size);

    // Get maximum colour value (normally 255)
    std::getline(imageFile, lineString);
    std::istringstream maxValueString(lineString);
    double maxImageValue;
    maxValueString >> maxImageValue;

    // Read pixels from file (as RGB, but only looking at Red channel, as R=G=B for greyscale images)
    char imageChar;
    for (int i = 0; i < size * size; i++) {
        std::getline(imageFile, lineString);
        std::istringstream valueString(lineString);
        double value;
        valueString >> value;

        // Convert colour value to height value
        double height = value * maxHeight / maxImageValue;
        heightfield[i / size][i % size] = height;
        std::getline(imageFile, lineString);
        std::getline(imageFile, lineString);
    }

    imageFile.close();

    *terrainSize = size;
    return heightfield;
}
