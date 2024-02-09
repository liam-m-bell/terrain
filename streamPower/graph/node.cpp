#include "node.h"

void Node::addEdge(Node *node){
    for (int i = 0; i < neighbours.size(); i++){
        if (neighbours[i] == node){
            return;
        }
    }
    neighbours.push_back(node);
}

void Node::removeEdge(Node *node){
    for (int i = 0; i < neighbours.size(); i++){
        if (neighbours[i] == node){
            neighbours.erase(neighbours.begin() + i);
            break;
        }
    }
}