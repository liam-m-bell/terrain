#ifndef LAKE_EDGE_H_
#define LAKE_EDGE_H_

#include <vector>
#include <tuple>

#include "stream_node.h"
#include "lake_node.h"

class StreamNode;

class LakeNode;

enum PassDirection {oneToTwo, twoToOne, none};

class LakeEdge{
public:
    float passHeight;
    StreamNode *passNode1;
    StreamNode *passNode2;

    LakeNode *lake1;
    LakeNode *lake2;

    PassDirection direction;

    LakeEdge()
	{
        
	}

    LakeEdge(StreamNode *n1, StreamNode *n2, float height);

    LakeNode *lowerLake();
    LakeNode *higherLake();
    StreamNode *lowerPass();
    StreamNode *higherPass();
};

class PassCompare{
    public:
        bool operator()(LakeEdge *a, LakeEdge *b){
            return (a->passHeight > b->passHeight);
        }
};

#endif