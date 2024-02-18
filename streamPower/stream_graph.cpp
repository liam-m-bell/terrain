#include "stream_graph.h"
#include "../core/vector.h"
#include "delaunay/CDT.h"
#include "poisson_disk_sampling.h"
#include <iostream>


#include "../core/noise.h"

float getInitalUplift(float x, float y){
    return 1.0f;
}

Vector circumcentreOfTriangle(Vector a, Vector b, Vector c){
    float t = a.len_sqr() - b.len_sqr();
    float u = a.len_sqr() - c.len_sqr();
    float J = ((a.x - b.x) * (a.y - c.y) - ((a.x - c.x) * (a.y - b.y))) * 2.0f;

    float x = (-(a.y - b.y) * u + (a.y - c.y) * t) / J;
    float y = ((a.x - b.x) * u - (a.x - c.x) * t) / J;
    return Vector(x, y);
}

// int minElement(float a, float b, float c){
//     float min = a;
//     float minIndex = 0;
    
//     if (b < min){
//         min = b;
//         minIndex = 1;
//     }
//     if (c < min){
//         minIndex = 2;
//     }
//     return minIndex;
// }

// int maxElement(float a, float b, float c){
//     float max = a;
//     float maxIndex = 0;
    
//     if (b > max){
//         max = b;
//         maxIndex = 1;
//     }
//     if (c < max){
//         maxIndex = 2;
//     }
//     return maxIndex;
// }

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
        float upl = (5.0f * pow(10.0f, -4));
        float height = fabs(warpedNoise(Vector(5.2f, 1.3f), 0.0f, Vector(v.x / (float)terrainSize, v.y / (float)terrainSize), 5, 2.0f, 0.5f, 40.0f));
        //float height = (Vector((float)terrainSize / 2, (float)terrainSize / 2) - v).length() * -0.1;
        nodes.push_back(StreamNode(v.x, v.y, height, upl));
        std::cout << height << "\n";
    }

    CDT::Triangulation<float> cdt;
    cdt.insertVertices(
        points.begin(),
        points.end(),
        [](const Vector& p){ return p.x; },
        [](const Vector& p){ return p.y; }
    );

    cdt.eraseSuperTriangle();

    for (auto tri : cdt.triangles){
        Vector centre = circumcentreOfTriangle(nodes[tri.vertices[0]].position, nodes[tri.vertices[1]].position, nodes[tri.vertices[2]].position);
        if (centre.x <= 0.0f || centre.y <= 0.0f || centre.x > (float)terrainSize || centre.y > (float)terrainSize){
            //Exclude
            continue;
        }

        nodes[tri.vertices[0]].addEdge(&nodes[tri.vertices[1]]);
        nodes[tri.vertices[0]].addEdge(&nodes[tri.vertices[2]]);
        
        nodes[tri.vertices[1]].addEdge(&nodes[tri.vertices[0]]);
        nodes[tri.vertices[1]].addEdge(&nodes[tri.vertices[2]]);

        nodes[tri.vertices[2]].addEdge(&nodes[tri.vertices[0]]);
        nodes[tri.vertices[2]].addEdge(&nodes[tri.vertices[1]]);

        edges.push_back(std::make_tuple(tri.vertices[0], tri.vertices[1]));
        edges.push_back(std::make_tuple(tri.vertices[1], tri.vertices[2]));
        edges.push_back(std::make_tuple(tri.vertices[2], tri.vertices[0]));

        Triangle face = {(unsigned int)tri.vertices[0], (unsigned int)tri.vertices[1], (unsigned int)tri.vertices[2]};
        triangles.push_back(face);
    }

    // Get boundary nodes
    float radiusSquare = (terrainSize / 2) - (6 * kRadius);
    for (int i = 0; i < edges.size(); i++){
        // if (fabs(nodes[std::get<0>(edges[i])].position.x) < radiusSquare && fabs(nodes[std::get<0>(edges[i])].position.y) < radiusSquare){
        //     continue;
        // }

        bool isBoundaryEdge = true;
        for (int j = 0; j < edges.size(); j++){
            bool sharedEdge = (std::get<0>(edges[i]) == std::get<0>(edges[j]) && std::get<1>(edges[i]) == std::get<1>(edges[j]))
                || (std::get<0>(edges[i]) == std::get<1>(edges[j]) && std::get<1>(edges[i]) == std::get<0>(edges[j]));

            if (i != j && sharedEdge){
                isBoundaryEdge = false;
                break;
            }
        }

        if (isBoundaryEdge){
            nodes[std::get<0>(edges[i])].boundaryNode = true;
            nodes[std::get<1>(edges[i])].boundaryNode = true;
            nodes[std::get<0>(edges[i])].height = 0.0f;
            nodes[std::get<1>(edges[i])].height = 0.0f;
        }
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
        unsigned int vertexIndicies[3] = {tri.v0, tri.v1, tri.v2};
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

    float sum = 0.0f;
    for (auto node : nodes){
        sum += node.voronoiArea;
        std::cout << node.voronoiArea << "\n";
    }
    std::cout << "Total:" << sum << "\n";
}

void StreamGraph::createStreamTrees(){
    for (int i = 0; i < nodes.size(); i++){
        StreamNode *node = &(nodes[i]);
        node->downstreamNode = 0;
        node->upstreamNodes.clear();
    }

    for (int i = 0; i < nodes.size(); i++){
        StreamNode *node = &(nodes[i]);
        StreamNode *lowest = 0;
        float lowestHeight = node->height;
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

void StreamGraph::updateNode(StreamNode *node, float dt){
    if (node->downstreamNode != 0){
        float horizontalDistance = (node->position - node->downstreamNode->position).length();
        float newHeight = (node->height + dt * (node->uplift + erosionConstant * pow(node->drainageArea, m) * node->downstreamNode->height / horizontalDistance)) /
                        (1 + erosionConstant * pow(node->drainageArea, m) * dt / horizontalDistance);

        node->height = newHeight;

        float maxSlope = tan(0.5);
        float slope = (node->height - node->downstreamNode->height) / horizontalDistance;
        if (slope > maxSlope){
            node->height = node->downstreamNode->height + horizontalDistance * maxSlope;
        }
        else if (slope < (-1.0f * maxSlope)){
            node->height = node->downstreamNode->height - horizontalDistance * maxSlope;
        }
    }

    for (int i = 0; i < node->upstreamNodes.size(); i++){
        updateNode(node->upstreamNodes[i], dt);
    }
}   

void StreamGraph::update(){
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
    for (int i = 0; i < nodes.size(); i++){
        if (nodes[i].boundaryNode){
            // Root
            updateNode(&nodes[i], timeStep);
        }
    }
}

bool areNeighbours(Node *a, Node *b){
     for (int i = 0; i < a->neighbours.size(); i++){
        if (a->neighbours[i] == b){
            return true;
        }
    }
    return false;
}

void StreamGraph::calculatePasses(){
    std::vector<LakeEdge> passes;
    std::vector<LakeEdge*> validPasses;

    int passCount = 0;
    for (int i = 0; i < edges.size(); i++){
        StreamNode *node1 = &(nodes[std::get<0>(edges[i])]);
        StreamNode *node2 = &(nodes[std::get<1>(edges[i])]);
        if (node1->lakeNode != node2->lakeNode){
            bool isPass = true;
            for (int l = 0; l < node1->lakeNode->upstreamNodes.size(); l++){
                for (int k = 0; k < node2->lakeNode->upstreamNodes.size(); k++){
                    if (areNeighbours(node1->lakeNode->upstreamNodes[l], node2->lakeNode->upstreamNodes[k])){
                        if (std::max(node1->height, node2->height) > std::max(node1->lakeNode->upstreamNodes[l]->height, node2->lakeNode->upstreamNodes[k]->height)){
                            isPass = false;
                            goto endLoop;
                        }
                    }
                }
            }
            endLoop:
            if (isPass){
                node1->lakeNode->addEdge(node2->lakeNode);
                node2->lakeNode->addEdge(node1->lakeNode);

                float passHeight = std::max(node1->height, node2->height);
                passes.push_back(LakeEdge(node1, node2, passHeight));

                //->lakeNode->passes.push_back(&passes[passCount]);
                //node2->lakeNode->passes.push_back(&passes[passCount]);
                //passCount++;
            }
        }
    }

    // Compute lake trees
    //std::vector<std::tuple<LakeNode*, LakeNode*, float, int>> candidates;
    std::vector<LakeEdge*> candidates;

    // Remove connections aways from river mouth
    for (int i = 0; i < lakeGraph.size(); i++){
        if (lakeGraph[i]->isRiverMouth){
            for (int j = 0; j < lakeGraph[i]->neighbours.size(); j++){
                if (!((LakeNode*)(lakeGraph[i]->neighbours[j]))->isRiverMouth){
                    //std::tuple<LakeNode*, LakeNode*, float, int> candidate = std::make_tuple((LakeNode*)(lakeGraph[i]->neighbours[j]), lakeGraph[i], std::get<2>((lakeGraph[i]->passes[j])), j);
                    
                    for (int p = 0; p < passes.size(); p++){
                        if (passes[p].lake1 == lakeGraph[i] && passes[p].lake2 == lakeGraph[i]->neighbours[j]){
                            passes[p].direction = twoToOne;
                            candidates.push_back(&passes[p]);

                        }
                        else if (passes[p].lake2 == lakeGraph[i] && passes[p].lake1 == lakeGraph[i]->neighbours[j]){
                            passes[p].direction = oneToTwo;
                            candidates.push_back(&passes[p]);
                        }
                    }
                } 
            }
            lakeGraph[i]->neighbours.clear();
        }
    }

    while (candidates.size() > 0){
        float minPassHeight = 1000000000;
        int minHeightPass;
        for (int i = 0; i < candidates.size(); i++){
            if (candidates[i]->passHeight < minPassHeight){
                minPassHeight = candidates[i]->passHeight;
                minHeightPass = i;
            }
        }

        LakeNode *lowerLake = candidates[minHeightPass]->lowerLake();
        LakeNode *upperLake = candidates[minHeightPass]->higherLake();

        validPasses.push_back(candidates[minHeightPass]);

        //lowerLake->upstreamLakes.push_back(std::make_tuple(std::get<0>(candidates[minHeightPass]), std::get<3>(candidates[minHeightPass])));

        // for (int j = 0; j < upperLake->neighbours.size(); j++){
        //     candidates.push_back(std::make_tuple((LakeNode*)(upperLake->neighbours[j]), upperLake, std::get<2>(upperLake->passes[j]), j));
        //     upperLake->neighbours.clear();
        // }

        for (int j = 0; j < upperLake->neighbours.size(); j++){
            if (!lakeGraph[j]->isRiverMouth){
                //std::tuple<LakeNode*, LakeNode*, float, int> candidate = std::make_tuple((LakeNode*)(lakeGraph[i]->neighbours[j]), lakeGraph[i], std::get<2>((lakeGraph[i]->passes[j])), j);
                
                for (int p = 0; p < passes.size(); p++){
                    if (passes[p].lake1 == lowerLake && passes[p].lake2 == upperLake){
                        passes[p].direction = twoToOne;
                        candidates.push_back(&passes[p]);

                    }
                    else if (passes[p].lake2 == lowerLake && passes[p].lake1 == upperLake){
                        passes[p].direction = oneToTwo;
                        candidates.push_back(&passes[p]);
                    }
                }
            }
        }
        
        upperLake->neighbours.clear();
        candidates.erase(candidates.begin() + minHeightPass);
    }

    for (int p = 0; p < validPasses.size(); p++){
        StreamNode *upperLakeRoot = validPasses[p]->higherLake()->streamNode;
        StreamNode *receiverNode = validPasses[p]->lowerPass();

        receiverNode->upstreamNodes.push_back(upperLakeRoot);
        upperLakeRoot->downstreamNode = receiverNode;
    }

    // // Update stream graph
    // for (int i = 0; i < lakeGraph.size(); i++){
    //     if (lakeGraph[i]->isRiverMouth){
    //         updateFromLakes(lakeGraph[i]);
    //     }
    // }
}

// void StreamGraph::updateFromLakes(LakeNode *lake){
//     for (int i = 0; i < lake->upstreamLakes.size(); i++){
//         // For each upstream lake connect a streamedge from upstream lake root to pass node of lower lake
//         LakeNode *upstreamLake = std::get<0>(lake->upstreamLakes[i]);
//         std::tuple<StreamNode*,StreamNode*,float> pass = upstreamLake->passes[std::get<1>(lake->upstreamLakes[i])];

//         StreamNode *passNode;
//         if (std::get<0>(pass)->lakeNode == lake){
//             passNode = std::get<0>(pass);
//         }
//         else if(std::get<0>(pass)->lakeNode == upstreamLake){
//             passNode = std::get<1>(pass);
//         }
//         else{
//             std::cout << "error";
//         }

//         passNode->upstreamNodes.push_back(upstreamLake->streamNode);

//         updateFromLakes(upstreamLake);
//     }
// }
