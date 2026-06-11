#include "Ghost.hpp"

Ghost::Ghost(int node, double speed) : nodeIndex_(node), lastNode_(-1), scared_(false), speed_(speed){
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
    const std::vector<int>& neighbors = graph.getAdj(nodeIndex_);

    if(neighbors.empty())
        return;

    // Elige un vecino al azar evitando devolverse por donde vino
    // (salvo que sea la unica salida). Asi no tiembla en un punto.
    auto pickRandom = [&](const std::vector<int>& opts) -> int {
        std::vector<int> filtered;
        for(int n : opts)
            if(n != lastNode_) filtered.push_back(n);
        const std::vector<int>& pool = filtered.empty() ? opts : filtered;
        std::uniform_int_distribution<int> d(0, (int)pool.size() - 1);
        return pool[d(random_)];
    };

    int from = nodeIndex_;

    //ASUSTADO: huye al azar
    if(scared_){
        nodeIndex_ = pickRandom(neighbors);
        lastNode_ = from;
        return;
    }

    //PERSECUCION: la mayoria de las veces persigue con BFS hacia Pacman
    std::uniform_int_distribution<int> dice(0, 100);
    if(dice(random_) <= GHOST_BFS_RATE){
        std::vector<int> shortPath = path.getShortestPath(nodeIndex_, pacmanNode);
        if(shortPath.size() >= 2){
            nodeIndex_ = shortPath[1];
            lastNode_ = from;
            return;
        }
    }

    //Si no persigue (o no hay camino), se mueve al azar sin devolverse
    nodeIndex_ = pickRandom(neighbors);
    lastNode_ = from;
}
  
