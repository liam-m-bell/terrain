#include <stdio.h>
#include <stdlib.h>

#include "mesh.h"

// Create a mesh from a heightfield
Mesh *createMeshFromHeightfield(float **heightfield, const int size){
    Mesh *mesh = (Mesh*)malloc(sizeof(Mesh));
    
    int vertexCount = size * size + 2 * size + 2 * (size - 2);
    Vector *vertices = (Vector*)malloc(vertexCount * sizeof(Vector));

    int faceCount = 2 * (size - 1) * (size - 1) + 4 * 2 * (size - 1) + 2;
    Triangle *faces = (Triangle*)malloc(faceCount * sizeof(Triangle));

    int i = 0;

    // Terrain mesh
    float min = 0;
    for (int z = 0; z < size; ++z){
        for (int x = 0; x < size; x++) {
            float elevation = heightfield[x][z];

            if (elevation < min){
                min = elevation;
            }

            vertices[i].x = x;
            vertices[i].y = elevation;
            vertices[i].z = z;
            i++;
        }
    }

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

    // Base and sides
    int baseTopLeft = size * size;
    int baseTopRight = baseTopLeft + size - 1;
    int baseBottomLeft = baseTopLeft + size + 2 * (size - 2);
    int baseBottomRight = baseBottomLeft + size - 1;

    for (int x = 0; x < size; x++) {
        vertices[i].x = x;
        vertices[i].y = min;
        vertices[i].z = 0;

        if (x < size - 1){
            faces[count].v0 = i;
            faces[count].v1 = x;
            faces[count].v2 = x + 1;
            count++;
            faces[count].v0 = i;
            faces[count].v1 = x + 1;
            faces[count].v2 = i + 1;
            count++;
        }
        i++;        
    }
    faces[count].v0 = baseTopLeft;
    faces[count].v1 = 0;
    faces[count].v2 = size;
    count++;
    faces[count].v0 = baseTopLeft;
    faces[count].v1 = size;
    faces[count].v2 = i;
    count++;
    for (int z = 1; z < size - 1; z++) {
        vertices[i].x = 0;
        vertices[i].y = min;
        vertices[i].z = z;
        
        faces[count].v0 = i;
        faces[count].v1 = size * z;
        faces[count].v2 = size * (z + 1);
        count++;
        
        faces[count].v0 = i;
        faces[count].v1 = size * (z + 1);
        if (z < size - 2){
            faces[count].v2 = i + 1;
        }
        else{
            faces[count].v2 = baseBottomLeft;
        }
        count++;
        i++; 
    }
    faces[count].v0 = baseTopRight;
    faces[count].v1 = size - 1;
    faces[count].v2 = 2 * size - 1;
    count++;
    faces[count].v0 = baseTopRight;
    faces[count].v1 = 2 * size - 1;
    faces[count].v2 = i;
    count++;
    for (int z = 1; z < size - 1; z++) {
        vertices[i].x = size - 1;
        vertices[i].y = min;
        vertices[i].z = z;

        faces[count].v0 = i;
        faces[count].v1 = size * z + size - 1;
        faces[count].v2 = size * (z + 1) + size - 1;
        count++;
        faces[count].v0 = i;
        faces[count].v1 = size * (z + 1) + size - 1;
        if (z < size - 2){
            faces[count].v2 = i + 1;
        }
        else{
            faces[count].v2 = baseBottomRight;
        }
        count++;
        i++;
    }
    for (int x = 0; x < size; x++) {
        vertices[i].x = x;
        vertices[i].y = min;
        vertices[i].z = size - 1;

        if (x < size - 1){
            faces[count].v0 = i;
            faces[count].v1 = size * (size - 1) + x;
            faces[count].v2 = size * (size - 1) + x + 1;
            count++;
            faces[count].v0 = i;
            faces[count].v1 = size * (size - 1) + x + 1;
            faces[count].v2 = i + 1;
            count++;
        }
        i++;
    }

    // Base
    faces[count].v0 = baseTopLeft;
    faces[count].v1 = baseTopRight;
    faces[count].v2 = baseBottomRight;
    count++;
    faces[count].v0 = baseTopLeft;
    faces[count].v1 = baseBottomRight;
    faces[count].v2 = baseBottomLeft;
    
    mesh->vertexCount = vertexCount;
    mesh->vertices = vertices;
    mesh->faceCount = faceCount;
    mesh->faces = faces;
    return mesh;
}

// Export mesh as OBJ file
void exportMeshAsObj(Mesh *mesh, const char *filename){
    Vector *vertices = mesh->vertices;
    Triangle *faces = mesh->faces;

    FILE* objFile = fopen(filename, "w");
    if (!objFile) {
        fprintf(stderr, "Error: Unable to open file for writing: %s\n", filename);
        return;
    }

    // Vertices
    for (int i = 0; i < mesh->vertexCount; i++) {
        fprintf(objFile, "v %f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
    }

    // Faces
    for (int i = 0; i < mesh->faceCount; i++) {
        fprintf(objFile, "f %d %d %d\n", faces[i].v0 + 1, faces[i].v1 + 1, faces[i].v2 + 1);
    }

    fclose(objFile);
}
