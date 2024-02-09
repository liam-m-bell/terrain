#ifndef STREAM_NODE_H_
#define STREAM_NODE_H_

#include "graph/node.h"

class StreamNode : public Node{
public:
    float x;
    float y;
    float height;
    float uplift;

    StreamNode(float x, float y, float height, float uplift){
        this->x = x;
        this->y = y;
        this->height = height;
        this->uplift = uplift;
    }

    StreamNode()
	{
        
	}
};

#endif