#ifndef STREAM_GRAPH_H_
#define STREAM_GRAPH_H_

#include "stream_node.h"
#include <vector>
#include <tuple>

#include "../core/mesh.h"

float getInitalUplift(float x, float y);
Vector circumcentreOfTriangle(Vector a, Vector b, Vector c);
float areaOfTriangle(Vector a, Vector b, Vector c);

class StreamGraph{
public:
    int terrainSize;
    int nodeCount;

    float **upliftField;
    int upliftFieldSize;

    float timeStep = 2.5 * pow(10.0f, 5);

    std::vector<StreamNode> nodes;
    std::vector<Triangle> triangles;
    std::vector<std::tuple<int, int>> edges;

    std::vector<LakeNode*> lakeGraph;

    StreamGraph(int nodeCount, int terrainSize, float **upliftField, int upliftFieldSize){
        this->nodeCount = nodeCount;
        this->upliftField = upliftField;
        this->terrainSize = terrainSize;
        this->upliftFieldSize =upliftFieldSize;
    }

    StreamGraph(){

    }
    
    void initialise();
    float getUplift(Vector p);
    void voronoiTessellation();
    void createStreamTrees();
    bool update();
    void updateNode(StreamNode *node, float dt);
    void calculatePasses();

    Mesh *createMesh();
    float **createHightfield(float precision, float sigma, float *maxHeight);
};

#endif
