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

    if (x - offset >= 0) {
        total += heightfield[x - offset][z];
        count++;
    }
    if (x + offset < size) {
        total += heightfield[x + offset][z];
        count++;
    }
    if (z - offset >= 0) {
        total += heightfield[x][z - offset];
        count++;
    }
    if (z + offset < size) {
        total += heightfield[x][z + offset];
        count++;
    }

    float average = total / (float)count;
    heightfield[x][z] = average + randomHeight(size) * scale;
}

void diamondStep(float **heightfield, const int size, int x, int z, int offset, float scale){
    float average = (heightfield[x-offset][z-offset] +
        heightfield[x+offset][z-offset] + 
        heightfield[x-offset][z+offset] +
        heightfield[x+offset][z+offset]) / 4.0f;
    
    heightfield[x][z] = average + randomHeight(size) * scale;
}

void diamondSquareStep(float **heightfield, const int size, int width, float scale){
    int half = width / 2;

    for (int x = half; x < size; x += width){
        for (int z = half; z < size; z += width){
            diamondStep(heightfield, size, x, z, half, scale);
        }
    }

    for (int x = half; x < size; x += width){
        for (int z = half; z < size; z += width){
            squareStep(heightfield, size, x, z - half, half, scale);
            squareStep(heightfield, size, x, z + half, half, scale);
            squareStep(heightfield, size, x - half, z, half, scale);
            squareStep(heightfield, size, x + half, z, half, scale);
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