#ifndef NODE_H_
#define NODE_H_

#include <vector>

class Node{
public:
    std::vector<Node*> neighbours;

    Node(){
        
	}

    void addEdge(Node *node);
    void removeEdge(Node *node);
};

#endif