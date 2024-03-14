#include "stream_node.h"

// Calculate drainage area of stream node recursively
float StreamNode::calculateDrainageArea(){
    float area = voronoiArea;
    for (StreamNode *upstreamNode : upstreamNodes){
        area += upstreamNode->calculateDrainageArea();
    }

    drainageArea = area;
    return area;
}

// Assign a stream node to a lake
void StreamNode::addToLake(LakeNode *lakeNode){
    for (StreamNode *upstreamNode : upstreamNodes){
        upstreamNode->addToLake(lakeNode);
    }

    this->lakeNode = lakeNode;
    lakeNode->upstreamNodes.push_back(this);
}