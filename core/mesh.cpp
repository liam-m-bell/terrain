#include <stdio.h>
#include <stdlib.h>

#include "mesh.h"

// Create a triangle from three vertex indicies
Triangle makeTriangle(unsigned int a, unsigned int b, unsigned int c){
    Triangle t;
    t.v0 = a;
    t.v1 = b;
    t.v2 = c;
    return t;
}

// Adds a vertex to a mesh
void addVertexToMesh(Mesh *mesh, double x, double y, double z){
    mesh->vertices[mesh->vertexCount] = Vector(x, y, z);
    (mesh->vertexCount)++;
}

// Adds a face to a mesh
void addFaceToMesh(Mesh *mesh, unsigned int a, unsigned int b, unsigned int c){
    mesh->faces[mesh->faceCount] = makeTriangle(a, b, c);
    (mesh->faceCount)++;
}

// Create a mesh from a heightfield
Mesh *createMeshFromHeightfield(double **heightfield, const int size){
    Mesh *mesh = (Mesh*)malloc(sizeof(Mesh));
    mesh->vertexCount = 0;
    mesh->faceCount = 0;
    
    int vertexCount = size * size + 2 * size + 2 * (size - 2);
    mesh->vertices = (Vector*)malloc(vertexCount * sizeof(Vector));

    int faceCount = 2 * (size - 1) * (size - 1) + 4 * 2 * (size - 1) + 2;
    mesh->faces = (Triangle*)malloc(faceCount * sizeof(Triangle));

    // Create verticies for every cell in the heightfield
    for (int z = 0; z < size; ++z){
        for (int x = 0; x < size; x++) {
            double elevation = heightfield[z][x];
            addVertexToMesh(mesh, x, elevation, z);
        }
    }

    // Create pairs of triangles between each of the verticies to create a tesselation
    for (int z = 0; z < size - 1; z++) {
        for (int x = 0; x < size - 1; x++) {
            int i = size * z + x;
            addFaceToMesh(mesh, i, i + 1, i + 1 + size);
            addFaceToMesh(mesh, i, i + 1 + size, i + size);
        }
    }

    // Add cut-out sides to the mesh
    int i = mesh->vertexCount;
    int baseTopLeft = size * size;
    int baseTopRight = baseTopLeft + size - 1;
    int baseBottomLeft = baseTopLeft + size + 2 * (size - 2);
    int baseBottomRight = baseBottomLeft + size - 1;

    for (int x = 0; x < size; x++) {
        addVertexToMesh(mesh, x, 0.0f, 0.0f);

        if (x < size - 1){
            addFaceToMesh(mesh, i, x, x + 1);
            addFaceToMesh(mesh, i, x + 1, i + 1);
        }
        i++;        
    }
    addFaceToMesh(mesh, baseTopLeft, 0, size);
    addFaceToMesh(mesh, baseTopLeft, size, i);

    for (int z = 1; z < size - 1; z++) {
        addVertexToMesh(mesh, 0, 0.0f, z);
        addFaceToMesh(mesh, i, size * z, size * (z + 1));

        if (z < size - 2){
            addFaceToMesh(mesh, i, size * (z + 1), i + 1);
        }
        else{
            addFaceToMesh(mesh, i, size * (z + 1), baseBottomLeft);
        }
        i++; 
    }
    addFaceToMesh(mesh, baseTopRight, size - 1, 2 * size - 1);
    addFaceToMesh(mesh, baseTopRight, 2 * size - 1, i);

    for (int z = 1; z < size - 1; z++) {
        addVertexToMesh(mesh, size - 1, 0.0f, z);
        addFaceToMesh(mesh, i, size * z + size - 1, size * (z + 1) + size - 1);

        if (z < size - 2){
            addFaceToMesh(mesh, i, size * (z + 1) + size - 1, i + 1);
        }
        else{
            addFaceToMesh(mesh, i, size * (z + 1) + size - 1, baseBottomRight);
        }
        i++;
    }
    for (int x = 0; x < size; x++) {
        addVertexToMesh(mesh, x, 0.0f, size - 1);

        if (x < size - 1){
            addFaceToMesh(mesh, i, size * (size - 1) + x, size * (size - 1) + x + 1);
            addFaceToMesh(mesh, i, size * (size - 1) + x + 1, i + 1);
        }
        i++;
    }

    // Create base of mesh with two triangles
    addFaceToMesh(mesh, baseTopLeft, baseTopRight, baseBottomRight);
    addFaceToMesh(mesh, baseTopLeft, baseBottomRight, baseBottomLeft);

    return mesh;
}

// Export mesh as OBJ (Wavefront) file at a specfified file path
void exportMeshAsObj(Mesh *mesh, const char *filename){
    Vector *vertices = mesh->vertices;
    Triangle *faces = mesh->faces;

    FILE* objFile = fopen(filename, "w");
    if (!objFile) {
        fprintf(stderr, "Error: Unable to open file for writing: %s\n", filename);
        return;
    }

    // Write vertices
    for (int i = 0; i < mesh->vertexCount; i++) {
        fprintf(objFile, "v %f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
    }

    // Write faces
    for (int i = 0; i < mesh->faceCount; i++) {
        fprintf(objFile, "f %d %d %d\n", faces[i].v0 + 1, faces[i].v1 + 1, faces[i].v2 + 1);
    }

    fclose(objFile);
}

// Frees the memory for a mesh
void freeMesh(Mesh *mesh){
    free(mesh->vertices);
    free(mesh->faces);
    free(mesh);
}