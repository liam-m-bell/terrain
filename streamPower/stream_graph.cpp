#include "stream_graph.h"
#include "../core/vector.h"
#include "delaunay/CDT.h"
#include "poisson_disk_sampling.h"
#include <iostream>

#include "../core/noise.h"

float getInitalUplift(float x, float y){
    return 1.0f;
}

void StreamGraph::initialise(){
    // Input parameters.
    auto kRadius = (float)terrainSize / sqrt((float)nodeCount);
    auto kXMin = std::array<float, 2>{{0.0f, 0.0f}};
    auto kXMax = std::array<float, 2>{{(float)terrainSize, (float)terrainSize}};

    // Samples returned as std::vector<std::array<float, 2>>.
    // Default seed and max sample attempts.
    std::vector<Vector> points;
    for (auto p : thinks::PoissonDiskSampling(kRadius, kXMin, kXMax)){
        points.push_back(Vector(p[0], p[1]));
    }

    for (Vector v : points){
        float height = (float)rand() / ((float)RAND_MAX + 1) * 4.0f;//warpedNoise(Vector(5.2f, 1.3f), 0.0f, Vector(v.x / (float)terrainSize, v.y / (float)terrainSize), 5, 2.0f, 0.5f, 40.0f);
        nodes.push_back(StreamNode(v.x, v.y, height, getInitalUplift(v.x, v.y)));
        std::cout << v.x << "," << v.y << "\n";
    }

    CDT::Triangulation<float> cdt;
    cdt.insertVertices(
        points.begin(),
        points.end(),
        [](const Vector& p){ return p.x; },
        [](const Vector& p){ return p.y; }
    );
    cdt.eraseSuperTriangle();

    int a = cdt.triangles.size();
    for (auto tri : cdt.triangles){
        nodes[tri.vertices[0]].addEdge(&nodes[tri.vertices[1]]);
        nodes[tri.vertices[0]].addEdge(&nodes[tri.vertices[2]]);
        
        nodes[tri.vertices[1]].addEdge(&nodes[tri.vertices[0]]);
        nodes[tri.vertices[1]].addEdge(&nodes[tri.vertices[2]]);

        nodes[tri.vertices[2]].addEdge(&nodes[tri.vertices[0]]);
        nodes[tri.vertices[2]].addEdge(&nodes[tri.vertices[1]]);

        Triangle face = {(unsigned int)tri.vertices[0], (unsigned int)tri.vertices[1], (unsigned int)tri.vertices[2]};
        triangles.push_back(face);
    }
}

Mesh* StreamGraph::createMesh(){
    Mesh *mesh = (Mesh*)malloc(sizeof(Mesh));

    mesh->vertexCount = nodes.size();
    mesh->vertices = (Vector*)malloc(mesh->vertexCount * sizeof(Vector));
    for (int i = 0; i < mesh->vertexCount; i++){
        mesh->vertices[i] = Vector(nodes[i].x, nodes[i].height, nodes[i].y);
    }

    mesh->faceCount = triangles.size();
    mesh->faces = (Triangle*)malloc(mesh->faceCount * sizeof(Triangle));
    for (int i = 0; i < mesh->faceCount; i++){
        mesh->faces[i] = triangles[i];
    }

    return mesh;
}