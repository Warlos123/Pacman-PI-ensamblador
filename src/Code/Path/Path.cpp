#include "Path.hpp"

Path::Path(Graph& graph) : graph_(graph){}

std::vector<int> Path::getShortestPath(int src, int dest){
    int total = graph_.getRows() * graph_.getCols();
    std::vector<bool> visited(total, false);
    std::vector<int> parent(total, -1);
    std::queue<int> queue;

    //BFS
    queue.push(src);
    visited[src] = true;

    while (!queue.empty()){
        int current = queue.front();
        queue.pop();

        if (current == dest) 
            break;

        for (int i : graph_.getAdj(current)) {
            if (!visited[i]) {
                visited[i] = true;
                parent[i] = current;
                queue.push(i);
            }
        }
    }

    //path, from dest to src
    std::vector<int> path;
    for (int node = dest; node != -1; node = parent[node]){
        path.push_back(node);
    }

    //path, from src to dest
    std::reverse(path.begin(), path.end());

    return path;
}
