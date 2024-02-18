#ifndef STREAM_NODE_H_
#define STREAM_NODE_H_

#include "graph/node.h"
#include "../core/vector.h"
#include "lake_node.h"

class StreamNode : public Node{
public:
    Vector position;
    float height;
    float uplift;
    float voronoiArea = 0;
    float drainageArea;
    bool boundaryNode = false;

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

    float calculateDrainageArea();
    void addToLake(LakeNode *lakeNode);
};

#endif