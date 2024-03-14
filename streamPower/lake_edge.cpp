#include "lake_edge.h"

LakeEdge::LakeEdge(StreamNode *n1, StreamNode *n2, float height){
    passNode1 = n1;
    passNode2 = n2;
    passHeight = height;
    lake1 = passNode1->lakeNode;
    lake2 = passNode2->lakeNode;
}

// Gets lake edges lower elevation neighbour
LakeNode* LakeEdge::lowerLake(){
    if (direction == oneToTwo){
        return lake2;
    }
    else if (direction == twoToOne){
        return lake1;
    }
    else{
        return (LakeNode*)0;
    }
}

// Gets lake edges lower elevation neighbour
LakeNode* LakeEdge::higherLake(){
    if (direction == oneToTwo){
        return lake1;
    }
    else if (direction == twoToOne){
        return lake2;
    }
    else{
        return (LakeNode*)0;
    }
}

// Gets lake edges lower elevation stream node of the pass
StreamNode* LakeEdge::lowerPass(){
    if (direction == oneToTwo){
        return passNode2;
    }
    else if (direction == twoToOne){
        return passNode1;
    }
    else{
        return (StreamNode*)0;
    }
}
