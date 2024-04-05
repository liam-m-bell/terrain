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

// Perform the stream power equation update on a node
bool StreamNode::update(float dt){
    float newHeight = height;
    if (downstreamNode != 0){
        float horizontalDistance = (position - downstreamNode->position).length();
        newHeight = (height + dt * (uplift + erosionConstant * pow(drainageArea, m) * downstreamNode->height / horizontalDistance)) /
                        (1 + erosionConstant * pow(drainageArea, m) * dt / horizontalDistance);

        // Thermal erosion
        float maxSlope = tan(0.5);
        float slope = (newHeight - downstreamNode->height) / horizontalDistance;
        if (slope > maxSlope){
            newHeight = downstreamNode->height + horizontalDistance * maxSlope;
        }
        else if (slope < (-1.0f * maxSlope)){
            newHeight = downstreamNode->height - horizontalDistance * maxSlope;
        }
    }
    
    // Update the children of the node in its stream tree
    bool childrenConverged = true;
    for (int i = 0; i < upstreamNodes.size(); i++){
        bool converged = upstreamNodes[i]->update(dt);
        childrenConverged = converged && childrenConverged;
    }

    float hightDifference = fabs(newHeight - height) / height;
    height = newHeight;

    return childrenConverged && (hightDifference < convergenceThreshold);
}   