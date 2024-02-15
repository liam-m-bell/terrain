#ifndef LAKE_NODE_H_
#define LAKE_NODE_H_

#include <vector>
#include <tuple>

#include "stream_node.h"

class StreamNode;

class LakeNode : public Node{
public:
    StreamNode *streamNode;
    bool isRiverMouth = false;

    std::vector<std::tuple<StreamNode*, StreamNode*, float>> passes;

    std::vector<StreamNode*> upstreamNodes;
    std::vector<LakeNode*> upstreamLakes;

    LakeNode()
	{
        
	}

    LakeNode(StreamNode *streamNode,bool isRiverMouth){
        this->streamNode = streamNode;
        this->isRiverMouth = isRiverMouth;
	}
};

#endif