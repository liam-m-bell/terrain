#ifndef STREAM_NODE_H_
#define STREAM_NODE_H_

#include "graph/node.h"
#include "../core/vector.h"
#include "lake_node.h"

class StreamNode : public Node{
public:
    const float m = 0.5f;
    const float erosionConstant = 5.61 * pow(10.0f, -7);
    const float convergenceThreshold = 0.2;

    Vector position;
    float height;
    float uplift;
    float voronoiArea = 0;
    float drainageArea;

    bool boundaryNode = false;

    std::vector<int> edgeShareCount;

    StreamNode *downstreamNode = 0;
    std::vector<StreamNode*> upstreamNodes;

    LakeNode *lakeNode;

    StreamNode(float x, float y, float height, float uplift){
        this->position = Vector(x, y);
        this->height = height;
        this->uplift = uplift;
    }

    StreamNode()
	{
        
	}

    bool addEdge(StreamNode *node);
    float calculateDrainageArea();
    void addToLake(LakeNode *lakeNode);
    bool update(float dt);
};

#endif