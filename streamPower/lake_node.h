#ifndef LAKE_NODE_H_
#define LAKE_NODE_H_

#include <vector>
#include <tuple>

#include "stream_node.h"
#include "lake_edge.h"

class StreamNode;

class LakeEdge;

class LakeNode : public Node{
public:
    StreamNode *streamNode;
    bool isRiverMouth = false;

    //std::vector<std::tuple<StreamNode*, StreamNode*, float>> passes;
    std::vector<LakeEdge*> passes;

    std::vector<StreamNode*> upstreamNodes;
    std::vector<std::tuple<LakeNode*, int>> upstreamLakes; //with pass number

    LakeNode()
	{
        
	}

    LakeNode(StreamNode *streamNode,bool isRiverMouth){
        this->streamNode = streamNode;
        this->isRiverMouth = isRiverMouth;
	}
};

#endif