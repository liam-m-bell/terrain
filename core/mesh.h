#ifndef MESH_H_
#define MESH_H_

#include "vector.h"

typedef struct{
    unsigned int v0, v1, v2;
} Triangle;

typedef struct{
    int vertexCount;
    Vector *vertices;

    int faceCount;
    Triangle *faces;
} Mesh;

Triangle makeTriangle(unsigned int a, unsigned int b, unsigned int c);
void addVertexToMesh(Mesh *mesh, double x, double y, double z);
void addFaceToMesh(Mesh *mesh, unsigned int a, unsigned int b, unsigned int c);
Mesh *createMeshFromHeightfield(double **heightfield, const int size);
void exportMeshAsObj(Mesh *mesh, const char *filename);
void freeMesh(Mesh *mesh);

#endif