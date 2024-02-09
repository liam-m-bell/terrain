#include "stream_node.h"
#include "graph/edge.h"
#include <vector>

#include "../core/mesh.h"

class StreamGraph{
public:
    int nodeCount;
    float **upliftMap;
    int terrainSize;

    std::vector<StreamNode> nodes;
    std::vector<Triangle> triangles;


    StreamGraph(int nodeCount, float **upliftMap, int terrainSize){
        this->nodeCount = nodeCount;
        this->upliftMap = upliftMap;
        this->terrainSize = terrainSize;
    }

    StreamGraph(){

    }
    
    void initialise();
    Mesh *createMesh();
    // void voronoiTessellation();
};
