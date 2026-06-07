#pragma once
#include <queue>
#include <algorithm>
#include "../Graph/Graph.hpp"

class Path{
private:
    Graph& graph_;

public:
    Path(Graph& graph);
    std::vector<int> getShortestPath(int src, int dest);
};