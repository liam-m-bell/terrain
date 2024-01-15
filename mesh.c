#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "diamond_square.h"

// Structure to represent a 3D vector
typedef struct{
    float x, y, z;
} Vector;

typedef struct{
    int v0, v1, v2;
} Triangle;

typedef struct{
    Vector *vertices;
    Triangle *faces;
} Mesh;

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

// Create a mesh from a heightfield
Mesh *createMeshFromHeightfield(float **heightfield, const int size){
    Mesh *mesh = (Mesh*)malloc(sizeof(Mesh));
    
    int vertexCount = size * size + 2 * size + 2 * (size -1);
    Vector *vertices = (Vector*)malloc(vertexCount * sizeof(Vector));

    float min = 0;

    int i = 0;
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; ++z) {
            float elevation = heightfield[x][z];

            if (elevation < min){
                min  = elevation;
            }

            vertices[i].x = x;
            vertices[i].y = elevation;
            vertices[i].z = z;
            i++;
        }
    }

    int faceCount = 2 * (size - 1) * (size - 1); //+ 4 * 2 * (size - 1) + 2;
    Triangle *faces = (Triangle*)malloc(faceCount * sizeof(Triangle));

    int count = 0;
    for (int z = 0; z < size - 1; z++) {
        for (int x = 0; x < size - 1; x++) {
            int i = size * z + x;
            faces[count].v0 = i;
            faces[count].v1 = i + 1;
            faces[count].v2 = i + 1 + size;
            count++;

            faces[count].v0 = i;
            faces[count].v1 = i + 1 + size;
            faces[count].v2 = i + size;
            count++;
        }
    }
    
    mesh->vertices = vertices;
    mesh->faces = faces;
    return mesh;
}

// Export mesh as OBJ file
void exportMeshAsObj(Mesh *mesh, const char *filename, const int size){
    Vector *vertices = mesh->vertices;
    Triangle *faces = mesh->faces;

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
    for (int i = 0; i < 2 * (size - 1) * (size - 1); i++) {
        fprintf(objFile, "f %d %d %d\n", faces[i].v0 + 1, faces[i].v1 + 1, faces[i].v2 + 1);
    }

    fclose(objFile);
}

int main(){
    srand(time(NULL));

    const int n = 5;
    const int size = pow(2, n) + 1;

    // Create heightfield
    float **heightfield = createHeightfield(size);

    // Generate heightfield
    diamondSquare(heightfield, size, 0.45);

    // Create mesh from heightfield
    Mesh *mesh = createMeshFromHeightfield(heightfield, size);

    // Export mesh as OBJ file
    const char* objFilename = "output.obj";
    exportMeshAsObj(mesh, objFilename, size);

    free(mesh);
    freeHeightfield(heightfield, size);

    return 0;
}
