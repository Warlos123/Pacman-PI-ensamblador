#include "MazeGenerator.hpp"


MazeGenerator::MazeGenerator(){
    std::random_device rd;
    this->random = std::mt19937(rd());
}

void MazeGenerator::addRandomEdge(Graph& graph){
    std::uniform_int_distribution<int> dist(0,100);

    for(int i = 0; i < graph.getRows() * graph.getCols(); i++){
        std::vector<int> neighbors = graph.getNeighbors(i);
        for(int n : neighbors){
            if(!graph.hasEdge(i, n) && dist(random) <= EXTRA_EDGE_PROB){ //EXTRA_EDGE_PROB% chance to create a new edge
                graph.addEdge(i,n);
            }   
        }
    }
}




void MazeGenerator::DFS(Graph& graph){
    int total = graph.getRows() * graph.getCols();

    std::vector<bool> visited(total, false);
    std::stack<int> stack;

    //Random node
    std::uniform_int_distribution<int> dist(0, total - 1);
    int start = dist(random);

    stack.push(start);
    visited[start] = true;

    
    while (!stack.empty()){
        int current = stack.top();

        std::vector<int> neighbors = graph.getNeighbors(current);
        std::vector<int> unvisited;

        for (int i : neighbors){
            if (!visited[i]) 
                unvisited.push_back(i);
        }


        if (unvisited.empty()){
            stack.pop(); 
        } 
        
        else{
            std::uniform_int_distribution<int> distCurrent(0, unvisited.size() - 1); //New distribution for random num, from 0 to the current unvisited # of nodes.
            int next = unvisited[distCurrent(random)];
            graph.addEdge(current, next);
            visited[next] = true;
            stack.push(next);
        }
    }

    //Opens extra paths inside the maze, creating new edges.
    addRandomEdge(graph);
}

