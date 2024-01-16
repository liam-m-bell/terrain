#include <stdlib.h>

float HEIGHT_SCALE = 0.4f;

// Diamond Square subdivision algorithm
float randomHeight(float max)
{
    max *= HEIGHT_SCALE;
    return (((float)rand() / RAND_MAX) * 2.0f * max) -  max;
}

void squareStep(float **heightfield, const int size, int x, int z, int offset, float scale){
    float total = 0.0f;
    int count = 0;

    if (z - offset >= 0) {
        total += heightfield[z - offset][x];
        count++;
    }
    if (z + offset < size) {
        total += heightfield[z + offset][x];
        count++;
    }
    if (x - offset >= 0) {
        total += heightfield[z][x - offset];
        count++;
    }
    if (x + offset < size) {
        total += heightfield[z][x + offset];
        count++;
    }

    float average = total / (float)count;
    heightfield[z][x] = average + randomHeight(size) * scale;
}

void diamondStep(float **heightfield, const int size, int z, int x, int offset, float scale){
    float average = (heightfield[z-offset][x-offset] +
        heightfield[z+offset][x-offset] + 
        heightfield[z-offset][x+offset] +
        heightfield[z+offset][x+offset]) / 4.0f;
    
    heightfield[z][x] = average + randomHeight(size) * scale;
}

void diamondSquareStep(float **heightfield, const int size, int width, float scale){
    int half = width / 2;

    for (int z = half; z < size; z += width){
        for (int x = half; x < size; x += width){
            diamondStep(heightfield, size, z, x, half, scale);
        }
    }

    for (int z = half; z < size; z += width){
        for (int x = half; x < size; x += width){
            squareStep(heightfield, size, z, x - half, half, scale);
            squareStep(heightfield, size, z, x + half, half, scale);
            squareStep(heightfield, size, z - half, x, half, scale);
            squareStep(heightfield, size, z + half, x, half, scale);
        }
    }
}

void diamondSquare(float **heightfield, const int size, const float roughness){
    // Initialise corners
    heightfield[0][0] = randomHeight(size);
    heightfield[0][size - 1] = randomHeight(size);
    heightfield[size - 1][0] = randomHeight(size);
    heightfield[size - 1][size - 1] = randomHeight(size);

    int width = size - 1;
    float scale = 1.0f;

    while (width > 1){
        diamondSquareStep(heightfield, size, width, scale);
        width /= 2;
        scale *= roughness; 
    }
}