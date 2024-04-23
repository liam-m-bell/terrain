#ifndef STREAM_NODE_H_
#define STREAM_NODE_H_

#include "graph/node.h"
#include "../core/vector.h"
#include "lake_node.h"

class StreamNode : public Node{
public:
    double m;
    double n;
    double erosionConstant;
    double convergenceThreshold;
    double talusAngle;

    Vector position;
    double height;
    double uplift;

    double rainfall;
    double voronoiArea = 0;
    double drainageArea;

    bool boundaryNode = false;
    std::vector<int> edgeShareCount;

    StreamNode *downstreamNode = 0;
    std::vector<StreamNode*> upstreamNodes;

    LakeNode *lakeNode;

    StreamNode(double x, double y, double height, double uplift, double m, double n, double k, double convergenceThreshold, double talusAngle, double rainfall){
        this->position = Vector(x, y);
        this->height = height;
        this->uplift = uplift;
        this->m = m;
        this->n = n;
        this->erosionConstant = k;
        this->convergenceThreshold = convergenceThreshold;
        this->talusAngle = talusAngle;
        this->rainfall = rainfall;
    }

    StreamNode()
	{
        
	}

    bool addEdge(StreamNode *node);
    double calculateDrainageArea();
    void addToLake(LakeNode *lakeNode);
    bool update(double dt);
};

#endif