#include "stream_graph.h"
#include "../core/vector.h"
#include "delaunay/CDT.h"
#include "poisson_disk_sampling.h"
#include <iostream>
#include <queue>
#include <chrono>
#include <map>
#include <stdint.h>
#include <utility>
#include <math.h>

#include "../core/noise.h"
#include "../core/heightfield.h"

Vector circumcentreOfTriangle(Vector a, Vector b, Vector c){
    double t = a.lengthSquared() - b.lengthSquared();
    double u = a.lengthSquared() - c.lengthSquared();
    double J = ((a.x - b.x) * (a.y - c.y) - ((a.x - c.x) * (a.y - b.y))) * 2.0f;

    double x = (-(a.y - b.y) * u + (a.y - c.y) * t) / J;
    double y = ((a.x - b.x) * u - (a.x - c.x) * t) / J;
    return Vector(x, y);
}

double StreamGraph::getUplift(Vector p){
    Vector upliftIndex = ((double)upliftFieldSize / (double)terrainSize) * p;
    double uplift = upliftField[(int)upliftIndex.x][(int)(upliftIndex.y)];
    return uplift;
    //return 5.5f * pow(10.0f, -4) * p.x / (double)terrainSize;
}

double StreamGraph::getRainfall(Vector p){
    if (variableRainfall){
        Vector rainfallIndex = ((double)rainfallFieldSize / (double)terrainSize) * p;
        double rainfall = rainfallField[(int)rainfallIndex.x][(int)(rainfallIndex.y)];
        return rainfall;
    }

    return 1.0f;
}

void StreamGraph::initialise(int nodeCount, double m, double n, double k, double convergenceThreshold, double minimumTalusAngle, double maximumTalusAngle){
    // Sample points for stream nodes using poisson disk samping
    // double radius = (double)terrainSize / sqrt((double)nodeCount);
    // std::vector<Vector> points = poissonDiskSampling(radius, Vector(terrainSize, terrainSize));

    // Input parameter
    auto kRadius = (double)terrainSize / sqrt((double)nodeCount * 1.62f);
    auto kXMin = std::array<double, 2>{{0.0f, 0.0f}};
    auto kXMax = std::array<double, 2>{{(double)terrainSize, (double)terrainSize}};

    // Samples returned as std::vector<std::array<double, 2>>.
    // Default seed and max sample attempts.
    std::vector<Vector> points;

    for (auto p : thinks::PoissonDiskSampling(kRadius, kXMin, kXMax)){
        points.push_back(Vector(p[0], p[1]));
    }

    for (Vector v : points){
        double uplift = getUplift(v);
        double height = 0.05f * fabs(warpedNoise(Vector(5.2f, 1.3f), 0.0f, Vector(v.x / (double)terrainSize, v.y / (double)terrainSize), 5, 2.0f, 0.5f, 40.0f));
        double talusAngle = (M_PI / 180.0f) * minimumTalusAngle + 1.25 * fabs(perlinNoise(Vector(v.x / (double)terrainSize, v.y / (double)terrainSize), 5, 2.0f, 0.5f, 1.0f));
        double rainfall = getRainfall(v);
        nodes.push_back(StreamNode(v.x, v.y, height, uplift, m, n, k, convergenceThreshold, talusAngle, rainfall));
    }   

    CDT::Triangulation<double> cdt;
    cdt.insertVertices(
        points.begin(),
        points.end(),
        [](const Vector& p){ return p.x; },
        [](const Vector& p){ return p.y; }
    );

    cdt.eraseSuperTriangle();

    // Create stream graph from triangulation
    for (auto tri : cdt.triangles){
        Vector centre = circumcentreOfTriangle(nodes[tri.vertices[0]].position, nodes[tri.vertices[1]].position, nodes[tri.vertices[2]].position);
        if (centre.x <= 0.0f || centre.y <= 0.0f || centre.x > (double)terrainSize || centre.y > (double)terrainSize){
            //Exclude if centre outside of terrain boundary
            continue;
        }

        nodes[tri.vertices[1]].addEdge(&nodes[tri.vertices[0]]);
        if (nodes[tri.vertices[0]].addEdge(&nodes[tri.vertices[1]])){
            edges.push_back(std::make_tuple(tri.vertices[0], tri.vertices[1]));
        }
        
        nodes[tri.vertices[2]].addEdge(&nodes[tri.vertices[0]]);
        if (nodes[tri.vertices[0]].addEdge(&nodes[tri.vertices[2]])){
            edges.push_back(std::make_tuple(tri.vertices[2], tri.vertices[0]));
        }
        
        nodes[tri.vertices[2]].addEdge(&nodes[tri.vertices[1]]);
        if (nodes[tri.vertices[1]].addEdge(&nodes[tri.vertices[2]])){    
            edges.push_back(std::make_tuple(tri.vertices[1], tri.vertices[2]));
        }

        Triangle face = {(unsigned int)tri.vertices[0], (unsigned int)tri.vertices[1], (unsigned int)tri.vertices[2]};
        triangles.push_back(face);
    }

    for (int i = 0; i < nodes.size(); i++){
        if (nodes[i].boundaryNode){
            continue;
        }
        for (int j = 0; j < nodes[i].neighbours.size(); j++){
            if (nodes[i].edgeShareCount[j] < 2 /* && nodes[i].uplift < 1.0f * pow(10.0f, -4)*/){
                nodes[i].boundaryNode = true;
                nodes[i].height = 0.0f;
                ((StreamNode*)nodes[i].neighbours[j])->boundaryNode = true;
                ((StreamNode*)nodes[i].neighbours[j])->height = 0.0f;
                continue;
            } 
        }
    }

    // Calculate voronoi areas for each node
    voronoiTessellation();

    std::cout << "Initialised stream graph\n";
    std::cout << "Node count: " << nodes.size() << "\n";
    std::cout.flush();
}

double areaOfTriangle(Vector a, Vector b, Vector c){
    return fabs(a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) / 2;
}

void StreamGraph::voronoiTessellation(){
    // For each triangle:
        
        // For each node in triangle:
            // Calculate perpendicular bisectors
            //Calculate area of quad (two triangles)
            // Add this to node's area

    for (Triangle tri : triangles){
        // Calculate circumcentre of triangle
        Vector circumcentre = circumcentreOfTriangle(nodes[tri.v0].position, nodes[tri.v1].position, nodes[tri.v2].position);
        unsigned int vertexIndicies[3] = {tri.v0, tri.v1, tri.v2};
        // For each node the triangle
        for (int i = 0; i < 3; i++){
            StreamNode *node = &nodes[vertexIndicies[i]];
            // For all neighbours of node
            for (int j = 0; j < 3; j++){
                if (j != i){
                    StreamNode neighbour = nodes[vertexIndicies[j]];
                    // Calculate midpoint of edge between node and neighbour
                    Vector midpoint = 0.5f * (node->position + neighbour.position);
                    // Calculate area of trianlge formed by node position, neighbour position, and circumcentre of triangle
                    double area = areaOfTriangle(node->position, midpoint, circumcentre);
                    node->voronoiArea += area;
                }
            }
        }
    }
}

// Calculate stream trees based on heights of stream nodes
void StreamGraph::createStreamTrees(){
    // Reset upstream and downstream stream tree connections for all nodes in stream graph
    for (int i = 0; i < nodes.size(); i++){
        StreamNode *node = &(nodes[i]);
        node->downstreamNode = 0;
        node->upstreamNodes.clear();
        node->lakeNode = 0;
    }

    // Create new stream trees
    for (int i = 0; i < nodes.size(); i++){
        StreamNode *node = &(nodes[i]);
        StreamNode *lowest = 0;
        double lowestHeight = node->height;
        
        // Find lowest neighbour to become downstream node
        for (Node *n : node->neighbours){
            StreamNode *neighbour = (StreamNode*)n;
            if (neighbour->height < lowestHeight){
                lowestHeight = neighbour->height;
                lowest = neighbour;
            }
        }

        if (lowest != 0){
            node->downstreamNode = lowest;
            lowest->upstreamNodes.push_back(node);
        }
    }
}

// Update stream graph
bool StreamGraph::update(){
    // Create stream trees
    createStreamTrees();

    // For each root, assign lakes
    for (int i = 0; i < nodes.size(); i++){
        if (nodes[i].downstreamNode == 0){
            // Root
            // Assign to lake
            LakeNode *lakeNode = new LakeNode(&nodes[i], nodes[i].boundaryNode);
            nodes[i].addToLake(lakeNode);
            lakeGraph.push_back(lakeNode);
        }
    }
    
    // Calculate passes
    calculatePasses();

    // For each root, calculate drainage areas.
    for (int i = 0; i < nodes.size(); i++){
        if (nodes[i].boundaryNode){
            // Root
            // Calculate drainage area
            nodes[i].calculateDrainageArea();
        }
    }

    // For each tree from root to leaf
    // Update node
    bool allConverged = true;
    for (int i = 0; i < nodes.size(); i++){
        if (nodes[i].boundaryNode){
        //if (nodes[i].downstreamNode == 0){
            // Root
            //printf("Root %d\n", nodes[i].number);
            bool converged = (&nodes[i])->update(timeStep);
            allConverged = converged && allConverged;
        }
    }

    //Free lakes
    for (int i = 0; i < lakeGraph.size(); i++){
        delete lakeGraph[i];
    }

    lakeGraph.clear();

    return allConverged;
}

void StreamGraph::calculatePasses(){
    std::vector<LakeEdge> passes;
    std::vector<LakeEdge> validPasses;

    // Find lake connections which could form passes
    for (int i = 0; i < edges.size(); i++){
        StreamNode *node1 = &(nodes[std::get<0>(edges[i])]);
        StreamNode *node2 = &(nodes[std::get<1>(edges[i])]);

        if (node1->lakeNode == node2->lakeNode){
            // Both nodes lead to same lake
            continue;
        }

        bool isPass = true;
        for (int j = 0; j < passes.size(); j++){
            LakeEdge p = passes[j];
            if ((p.lake1 == node1->lakeNode && p.lake2 == node2->lakeNode) || (p.lake1 == node2->lakeNode && p.lake2 == node1->lakeNode)){
                isPass = false;
                double height = std::max(node1->height, node2->height);
                if (height < p.passHeight){
                    isPass = true;
                    passes.erase(passes.begin() + j);
                }
                break;
            }
        }

        if (isPass){
            // Add connection in lake graph
            node1->lakeNode->addEdge(node2->lakeNode);
            node2->lakeNode->addEdge(node1->lakeNode);
            double passHeight = std::max(node1->height, node2->height);
            passes.push_back(LakeEdge(node1, node2, passHeight));
        }
    }

    // Compute lake trees
    std::priority_queue<LakeEdge*, std::vector<LakeEdge*>, PassCompare> candidates; 

    // Remove connections aways from river mouth (no flow upstream)
    int interiorNodeCount = 0;
    for (int i = 0; i < lakeGraph.size(); i++){
        if (lakeGraph[i]->isRiverMouth){
            for (int j = 0; j < lakeGraph[i]->neighbours.size(); j++){
                if (!((LakeNode*)(lakeGraph[i]->neighbours[j]))->isRiverMouth){
                    for (int p = 0; p < passes.size(); p++){
                        // Set direciton of lake flow for lake trees
                        if (passes[p].lake1 == lakeGraph[i] && passes[p].lake2 == lakeGraph[i]->neighbours[j]){
                            passes[p].direction = twoToOne;
                            candidates.push(&passes[p]);

                        }
                        else if (passes[p].lake2 == lakeGraph[i] && passes[p].lake1 == lakeGraph[i]->neighbours[j]){
                            passes[p].direction = oneToTwo;
                            candidates.push(&passes[p]);
                        }
                    }
                } 
            }
            lakeGraph[i]->neighbours.clear();
        }
        else{
            interiorNodeCount++;
        }
    }

    // Build valid lake tree
    buildLakeTree:
    while (!candidates.empty()){
        // // Find candidate pass with minimum height
        LakeEdge *minHeightPass = candidates.top();
        candidates.pop();

        LakeNode *lowerLake = minHeightPass->lowerLake();
        LakeNode *upperLake = minHeightPass->higherLake();

        for (int i = 0; i < validPasses.size(); i++){
            if (validPasses[i].higherLake() == upperLake){
                // Cannot have lake flowing in two differnt directions
                goto buildLakeTree;
            }
        }

        validPasses.push_back(*minHeightPass);

        // Add neighbours of upper lake as candidates for next pass (if not river mouth)
        for (int j = 0; j < upperLake->neighbours.size(); j++){
            if (!((LakeNode*)(upperLake->neighbours[j]))->isRiverMouth){
                for (int p = 0; p < passes.size(); p++){
                    // Set direciton of lake flow for lake tree
                    if (passes[p].lake1 == upperLake && passes[p].lake2 == upperLake->neighbours[j]){
                        passes[p].direction = twoToOne;
                        candidates.push(&passes[p]);

                    }
                    else if (passes[p].lake2 == upperLake && passes[p].lake1 == upperLake->neighbours[j]){
                        passes[p].direction = oneToTwo;
                        candidates.push(&passes[p]);
                    }
                }
            }
        }
        
        upperLake->neighbours.clear();
    }

    // Update stream graph with new connections formed by the lake trees
    for (int p = 0; p < validPasses.size(); p++){
        StreamNode *upperLakeRoot = validPasses[p].higherLake()->streamNode;
        StreamNode *receiverNode = validPasses[p].lowerPass();

        receiverNode->upstreamNodes.push_back(upperLakeRoot);
        upperLakeRoot->downstreamNode = receiverNode;
    }
}

// Create mesh of stream graph
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

double** StreamGraph::createHightfield(double precision, double sigma, double *maxHeight){
    int arraySize = (int)(terrainSize / precision);
    double **heightfield = createHeightfield(arraySize);
    double **kernelSum = createHeightfield(arraySize);

    int range = (int)(4.0f * sigma);

    for (int n = 0; n < nodes.size(); n++){
        int nodeX = (int)(nodes[n].position.x / precision);
        int nodeZ = (int)(nodes[n].position.y / precision);
        
        for (int i = std::max(nodeX - range, 0); i < std::min(nodeX + range, arraySize); i++) {
            for (int j = std::max(nodeZ - range, 0); j < std::min(nodeZ + range, arraySize); j++) {
                double distanceSquared = pow(i - nodeX, 2) + pow(j - nodeZ, 2);
                double gaussian = exp(-distanceSquared / (2 * pow(sigma, 2)));
                heightfield[i][j] += (nodes[n].height / precision) * gaussian;
                kernelSum[i][j] += gaussian;
            }
        }
    }

    // Normalise
    double max = 0.0f;
    for (int i = 0; i < arraySize; i++) {
        for (int j = 0; j < arraySize; j++) {
            heightfield[i][j] /= kernelSum[i][j];
            if (heightfield[i][j] > max){
                max= heightfield[i][j];
            }
        }
    }

    freeHeightfield(kernelSum, arraySize);
    *maxHeight = max;

    return heightfield;
}