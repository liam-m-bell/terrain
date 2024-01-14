#include <stdio.h>
#include <stdlib.h>

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
Vector *createMeshFromHeightfield(float **heightfield, const int size) {
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
void exportMeshAsObj(Vector *vertices, const char *filename, const int size) {
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

int main() {
    const int size = 100;

    // Create heightfield
    float **heightfield = createHeightfield(size);

    // Generate heightfield
    generateRandomHeightfield(heightfield, size);

    // Create mesh from heightfield
    Vector *mesh = createMeshFromHeightfield(heightfield, size);

    // Export mesh as OBJ file
    const char* objFilename = "output.obj";
    exportMeshAsObj(mesh, objFilename, size);

    free(mesh);
    freeHeightfield(heightfield, size);

    return 0;
}
