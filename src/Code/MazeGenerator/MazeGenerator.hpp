#pragma once
#include <random>
#include <stack>
#include <queue>
#include "../Graph/Graph.hpp"
#include"../Constants.hpp"

class MazeGenerator{
private:
    std::mt19937 random;

    void addRandomEdge(Graph& graph);
public:
    MazeGenerator();
    void DFS(Graph& graph);



};