#pragma once
#include <random>
//#include <SFML/System/Clock.hpp>
//#include <SFML/System/Time.hpp> 
#include "../Graph/Graph.hpp"
#include "../Path/Path.hpp"

class Ghost {
private:
    int nodeIndex_;
    bool scared_;
    double speed_;
    std::mt19937 random_;


public:
    Ghost(int node, double speed);

    int getNodeIndex() const;
    void setNodeIndex(int node);

    bool isScared() const;
    void setScared(bool a);

    double getSpeed() const;

    void move(Graph& graph, Path& path, int pacmanNode);
};