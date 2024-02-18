#include "stream_node.h"

float StreamNode::calculateDrainageArea(){
    float area = voronoiArea;
    for (StreamNode *upstreamNode : upstreamNodes){
        area += upstreamNode->calculateDrainageArea();
    }

    drainageArea = area;
    return area;
}

void StreamNode::addToLake(LakeNode *lakeNode){
    for (StreamNode *upstreamNode : upstreamNodes){
        upstreamNode->addToLake(lakeNode);
    }

    this->lakeNode = lakeNode;
    lakeNode->upstreamNodes.push_back(this);
}