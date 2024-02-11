#ifndef STREAM_NODE_H_
#define STREAM_NODE_H_

#include "graph/node.h"
#include "../core/vector.h"

class StreamNode : public Node{
public:
    Vector position;
    float height;
    float uplift;
    float voronoiArea = 0;

    StreamNode(float x, float y, float height, float uplift){
        this->position = Vector(x, y);
        this->height = height;
        this->uplift = uplift;
    }

    StreamNode()
	{
        
	}
};

#endif