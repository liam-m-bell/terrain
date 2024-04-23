#ifndef LAKE_NODE_H_
#define LAKE_NODE_H_

#include <vector>

#include "stream_node.h"
#include "lake_edge.h"

class StreamNode;

class LakeEdge;

class LakeNode : public Node{
public:
    StreamNode *streamNode;
    bool isRiverMouth = false;

    std::vector<LakeEdge*> passes;

    LakeNode()
	{
        
	}

    LakeNode(StreamNode *streamNode,bool isRiverMouth){
        this->streamNode = streamNode;
        this->isRiverMouth = isRiverMouth;
	}
};

#endif