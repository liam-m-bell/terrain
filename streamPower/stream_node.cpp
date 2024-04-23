#include "stream_node.h"

// Add an edge to the stream graph
bool StreamNode::addEdge(StreamNode *node){
    for (int i = 0; i < neighbours.size(); i++){
        if (neighbours[i] == node){
            edgeShareCount[i]++;
            return false;
        }
    }
    neighbours.push_back(node);
    edgeShareCount.push_back(1);
    return true;
}

// Calculate drainage area of stream node recursively
double StreamNode::calculateDrainageArea(){
    double area = voronoiArea * rainfall;
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
}

// Perform the stream power equation update on a node
bool StreamNode::update(double dt){
    double newHeight = height;
    if (downstreamNode != 0){
        // Apply the 
        double horizontalDistance = (position - downstreamNode->position).length();
        newHeight = (height + dt * (uplift + erosionConstant * pow(drainageArea, m) * downstreamNode->height / horizontalDistance)) /
                        (1 + erosionConstant * pow(drainageArea, m) * dt / horizontalDistance);

        // Apply correction based on thermal erosion
        double maxSlope = tan(talusAngle);
        double slope = (newHeight - downstreamNode->height) / horizontalDistance;
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

    double hightDifference = fabs(newHeight - height) / height;
    height = newHeight;

    // Check for convergence of this node, and its children
    return childrenConverged && (hightDifference < convergenceThreshold);
}   