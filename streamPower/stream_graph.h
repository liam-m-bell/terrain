#include "stream_node.h"
#include "graph/edge.h"
#include <vector>
#include <tuple>

#include "../core/mesh.h"

class StreamGraph{
public:
    int nodeCount;
    float **upliftMap;
    int terrainSize;

    float m = 0.5f;
    float erosionConstant = 5.61 * pow(10.0f, -7);
    float timeStep = 2.5 * pow(10.0f, 5);

    std::vector<StreamNode> nodes;
    std::vector<Triangle> triangles;
    std::vector<std::tuple<int, int>> edges;

    std::vector<LakeNode*> lakeGraph;

    StreamGraph(int nodeCount, float **upliftMap, int terrainSize){
        this->nodeCount = nodeCount;
        this->upliftMap = upliftMap;
        this->terrainSize = terrainSize;
    }

    StreamGraph(){

    }
    
    void initialise();
    Mesh *createMesh();
    void voronoiTessellation();
    void calculateBoundary();
    void createStreamTrees();
    void update();
    void updateNode(StreamNode *node, float dt);
    void calculatePasses();
    void updateFromLakes(LakeNode *lake);
};
