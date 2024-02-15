#ifndef EDGE_H_
#define EDGE_H_

#include <vector>


class Edge{
public:
    Node *node1;
    Node *node2;

    Edge(Node *a, Node *b){
        node1 = a;
        node2 = b;
    }

    Edge(){
        
	}
    
    Node* GetOppositeNode(Node *node){
        if (node == node1){
            return node1;
        }
        else if (node == node2){
            return node2;
        }
        else{
            return NULL;
        }
    }
};

#endif