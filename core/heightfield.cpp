#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

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

// Adapted from https://rosettacode.org/wiki/Bitmap/Write_a_PPM_file
void outputHeightfieldAsImage(float **a, const int size, const float maxHeight, char *filename){
    FILE *imageFile = fopen(filename, "wb");
    fprintf(imageFile, "P6\n%d %d\n255\n", size, size);

    for (int z = 0; z < size; z++){
        for (int x = 0; x < size; x++){
            unsigned char color[3];
            unsigned char value = (char)(a[z][x] * 255.0f / maxHeight);
            color[0] = value;
            color[1] = value;
            color[2] = value;
            fwrite(color, 1, 3, imageFile);
        }
    }

    fclose(imageFile);
}

int importImageAsHeightfield(char *filename, float ***a, const float maxHeight){
    int size = 0;
    std::ifstream imageFile(filename);
    
    std::string lineString;

    std::getline(imageFile, lineString);
    if (lineString != "P6"){ 
        return 0;
    }

    std::getline(imageFile, lineString);
    while (lineString[0] == '#') {
        std::getline(imageFile, lineString);
    }

    std::istringstream dimensions(lineString);

    int width, height;
    
    dimensions >> width;
    dimensions >> height;

    if (width != height){
        return 0;
    }

    size = width;
    float **heightfield = createHeightfield(size);

    std::getline(imageFile, lineString);
    std::istringstream maxValueString(lineString);
    float maxImageValue;
    maxValueString >> maxImageValue;

    char imageChar;
    for (int i = 0; i < size * size; i++) {
        imageFile.read(&imageChar, 1);
        unsigned char uImageChar = (unsigned char)imageChar;
        float value = (float)uImageChar * maxHeight / maxImageValue;
        heightfield[i / size][i % size] = value;
        std::cout << imageChar << " ";
        std::cout << i / size << " ";
        std::cout << i % size << "\n";
        imageFile.read(&imageChar, 2);
        //std::cout << imageChar << "\n";
    }

    imageFile.close();

    *a = heightfield;
    return size;
}
