#include "node.h"

// Add edge to the graph
void Node::addEdge(Node *node){
    for (int i = 0; i < neighbours.size(); i++){
        if (neighbours[i] == node){
            return;
        }
    }
    neighbours.push_back(node);
}

// Remove edge form the graph
void Node::removeEdge(Node *node){
    for (int i = 0; i < neighbours.size(); i++){
        if (neighbours[i] == node){
            neighbours.erase(neighbours.begin() + i);
            break;
        }
    }
}

// Check is another node is a neighbour
bool Node::isNeighbour(Node *a){
     for (int i = 0; i < neighbours.size(); i++){
        if (neighbours[i] == a){
            return true;
        }
    }
    return false;
}