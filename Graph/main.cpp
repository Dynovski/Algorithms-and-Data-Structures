#include <iostream>
#include "Graph.h"


int main()
{
    size_t num_of_vertices, v, u;

    std::cin >> num_of_vertices;

    aisdi::Graph G(num_of_vertices);

    while(std::cin >> v)
    {
        std::cin >> u;
        G.createEdge(v, u);
    }

    std::vector<std::pair<size_t, size_t>> result = G.findWideBridges();
    
    for(auto x: result)
    {
        std::cout << x.first << " " << x.second << std:: endl;
    }

    return 0;
}