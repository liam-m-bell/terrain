#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float MAX_HEIGHT = 600.0f;

// Structure to represent a 3D vector
typedef struct {
    float x, y, z;
} Vector;

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

float **generateRandomHeightfield(float **heightfield, const int size){
    for (int x = 0; x < size; x++) { 
        for (int z = 0; z < size; ++z) {
            float elevation = rand() / RAND_MAX;
            heightfield[x][z] = elevation;
        }
    }
}

// Create a mesh from a heightfield
Vector *createMeshFromHeightfield(float **heightfield, const int size){
    Vector *vertices = (Vector*)malloc(size * size * sizeof(Vector));

    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; ++z) {
            int i = size * x + z;
            vertices[i].x = x;
            vertices[i].y = heightfield[x][z];
            vertices[i].z = z;
        }
    }

    return vertices;
}

// Export mesh as OBJ file
void exportMeshAsObj(Vector *vertices, const char *filename, const int size){
    FILE* objFile = fopen(filename, "w");
    if (!objFile) {
        fprintf(stderr, "Error: Unable to open file for writing: %s\n", filename);
        return;
    }

    // Vertices
    for (int i = 0; i < size * size; i++) {
        fprintf(objFile, "v %f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
    }

    // Faces
    for (int x = 0; x < size - 1; x++) {
        for (int z = 0; z < size - 1; z++) {
            int i = size * x + z;
            fprintf(objFile, "f %d %d %d\n", i + 1, i + 2, i + 2 + size);
            fprintf(objFile, "f %d %d %d\n", i + 1, i + 2 + size, i + 1 + size);
        }
    }

    fclose(objFile);
}

// Diamond Square subdivision algorithm
float randomHeight()
{
    return (((float)rand() / RAND_MAX) * 2.0f * MAX_HEIGHT) -  MAX_HEIGHT;
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
    heightfield[x][z] = average + randomHeight() * scale;
}

void diamondStep(float **heightfield, const int size, int x, int z, int offset, float scale){
    float average = (heightfield[x-offset][z-offset] +
        heightfield[x+offset][z-offset] + 
        heightfield[x-offset][z+offset] +
        heightfield[x+offset][z+offset]) / 4.0f;
    
    heightfield[x][z] = average + randomHeight() * scale;
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
    heightfield[0][0] = randomHeight();
    heightfield[0][size - 1] = randomHeight();
    heightfield[size - 1][0] = randomHeight();
    heightfield[size - 1][size - 1] = randomHeight();

    int width = size - 1;
    float scale = 1.0f;

    while (width > 1){
        diamondSquareStep(heightfield, size, width, scale);
        width /= 2;
        scale *= roughness; 
    }
}

int main(){
    const int n = 10;
    const int size = pow(2, n) + 1;

    // Create heightfield
    float **heightfield = createHeightfield(size);

    // Generate heightfield
    //generateRandomHeightfield(heightfield, size);
    diamondSquare(heightfield, size, 0.4);

    // Create mesh from heightfield
    Vector *mesh = createMeshFromHeightfield(heightfield, size);

    // Export mesh as OBJ file
    const char* objFilename = "output.obj";
    exportMeshAsObj(mesh, objFilename, size);

    free(mesh);
    freeHeightfield(heightfield, size);

    return 0;
}
