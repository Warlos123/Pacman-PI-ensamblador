#include "Ghost.hpp"

Ghost::Ghost(int node, double speed) : nodeIndex_(node), scared_(false), speed_(speed){
    std::random_device rd;
    random_ = std::mt19937(rd());
}


int Ghost::getNodeIndex() const{
    return nodeIndex_;
}

void Ghost::setNodeIndex(int node){
    nodeIndex_ = node;
}


bool Ghost::isScared() const{
    return scared_;
}

void Ghost::setScared(bool a){
    scared_ = a;
}

double Ghost::getSpeed() const{
    return speed_;
}




void Ghost::move(Graph& graph, Path& path, int pacmanNode){
    std::vector<int> neighbors = graph.getAdj(nodeIndex_);

    if(neighbors.empty()) 
        return;


    if(scared_){
        std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);
        nodeIndex_ = neighbors[dist(random_)];
        return;
    }

    std::uniform_int_distribution<int> dist(0, 100);

    //BEST PATH
    if(dist(random_) <= GHOST_BFS_RATE){
        std::vector<int> shortPath = path.getShortestPath(nodeIndex_, pacmanNode);
        if (shortPath.size() >= 2)
            nodeIndex_ = shortPath[1];
    } 

    //RANDOM PATH
    else{
        std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);
        nodeIndex_ = neighbors[dist(random_)];
    }
}
  
