#include "stream_node.h"

float StreamNode::calculateDrainageArea(LakeNode *lakeNode){
    float area = voronoiArea;
    for (StreamNode *upstreamNode : upstreamNodes){
        area += upstreamNode->calculateDrainageArea(lakeNode);
    }

    drainageArea = area;
    this->lakeNode = lakeNode;
    lakeNode->upstreamNodes.push_back(this);
    return area;
}