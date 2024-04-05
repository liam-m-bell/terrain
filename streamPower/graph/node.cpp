#include "node.h"

// Add edge to the graph, true if added, false if already present
bool Node::addEdge(Node *node){
    for (int i = 0; i < neighbours.size(); i++){
        if (neighbours[i] == node){
            return false;
        }
    }
    neighbours.push_back(node);
    return true;
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