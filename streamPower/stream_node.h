#ifndef STREAM_NODE_H_
#define STREAM_NODE_H_

#include "graph/node.h"
#include "../core/vector.h"
#include "lake_node.h"

class StreamNode : public Node{
public:
    float m;
    float n;
    float erosionConstant;
    float convergenceThreshold;
    float talusAngle;

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

    StreamNode(float x, float y, float height, float uplift, float m, float n, float k, float convergenceThreshold, float talusAngle){
        this->position = Vector(x, y);
        this->height = height;
        this->uplift = uplift;
        this->m = m;
        this->n = n;
        this->erosionConstant = k;
        this->convergenceThreshold = convergenceThreshold;
        this->talusAngle = talusAngle;
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