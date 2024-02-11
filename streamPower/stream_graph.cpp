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

    voronoiTessellation();
}

Mesh* StreamGraph::createMesh(){
    Mesh *mesh = (Mesh*)malloc(sizeof(Mesh));

    mesh->vertexCount = nodes.size();
    mesh->vertices = (Vector*)malloc(mesh->vertexCount * sizeof(Vector));
    for (int i = 0; i < mesh->vertexCount; i++){
        mesh->vertices[i] = Vector(nodes[i].position.x, nodes[i].height, nodes[i].position.y);
    }

    mesh->faceCount = triangles.size();
    mesh->faces = (Triangle*)malloc(mesh->faceCount * sizeof(Triangle));
    for (int i = 0; i < mesh->faceCount; i++){
        mesh->faces[i] = triangles[i];
    }

    return mesh;
}

Vector circumcentreOfTriangle(Vector a, Vector b, Vector c){
    float t = a.len_sqr() - b.len_sqr();
    float u = a.len_sqr() - c.len_sqr();
    float J = ((a.x - b.x) * (a.y - c.y) - ((a.x - c.x) * (a.y - b.y))) / 2.0f;

    Vector centre = (-(a - b) * u + (a - c) * t) * J;
    return centre;
}

float areaOfTriangle(Vector a, Vector b, Vector c){
    return fabs(a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) / 2;
}

void StreamGraph::voronoiTessellation(){
    // For each triangle:
        //calculate circumference
        // For each node in triangle:
            // Calculate perpendicular bisectors
            //Calculate area of quad (two triangles)
            // Add this to node's area

    for (Triangle tri : triangles){
        Vector circumcentre = circumcentreOfTriangle(nodes[tri.v0].position, nodes[tri.v1].position, nodes[tri.v2].position);
        int vertexIndicies[3] = {tri.v0, tri.v1, tri.v2};
        for (int j = 0; j < 3; j++){
            StreamNode *node = &nodes[vertexIndicies[j]];
            for (int k = 0; k < 3; k++){
                if (k != j){
                    StreamNode neighbour = nodes[vertexIndicies[k]];
                    Vector midpoint = 0.5f * (node->position + neighbour.position);
                    float area = areaOfTriangle(node->position, midpoint, circumcentre);
                    node->voronoiArea += area;
                }
            }
        }
    }

    for (auto node : nodes){
        std::cout << node.voronoiArea << "\n";
    }
}