/* THIS CODE IS NOT THE MOST OPTIMIZED VERSION, FURTHER OPTIMIZATION CAN BE DONE BY ADDING SOME COUNTING VARIABLES TO REDUCE MANY LOOPS ITERATIONS, I DIDN'T HAVE ENOUGH TIME FOR IT */

#ifndef AISDI_GRAPH_H
#define AISDI_GRAPH_H

#include <vector>


namespace aisdi
{
    class Graph
    {
    public:
        using size_type = std::size_t;
        using edge = std::pair<size_type, size_type>;
    private:
        enum Possible_states {NOT_VISITED, VISITED, GHOST};                    // states in DFS search, GHOST is 'invisible' for one DFS iteration

        class Vertex
        {
        public:
            size_type vertex_id;
            size_type dfs_id = 0;                                              // always higher than 1

            /* low is MIN{dfs_id of current vertex, dfs_id of vertex connected with current vertex by back edge, low of each child in spanning tree} */
            
            /* at the beginning low == dfs_id in visited vertex, it gets lower when child has back edge
             * if low >= dfs_id in current vertex, it means, that vertex is articulation point in graph(deleting it makes graph disconnected)
             * because if low of child >= dfs_id of current vertex, it means that child has no back edge, the only way to the rest of a graph 
             * leads through its parent */

            size_type low = 0;                                                 // used to tell if vertex is articulation point
            Possible_states state = NOT_VISITED;
            std::vector<size_type> adjacent_vertices;

            void setAdjacentTo(size_type vertex)
            {
                adjacent_vertices.push_back(vertex);
            }
        };

        size_type num_of_vertices;
        std::vector<Vertex> vector_of_vertices;

        void clearAllStates()
        {
            for(size_type i = 0; i < num_of_vertices; i++)
                vector_of_vertices[i].state = NOT_VISITED;
        }

        bool vertexWasNotVisited(size_type & vertex) const
        {
            return vector_of_vertices[vertex].state == NOT_VISITED;
        }

        bool vertexIsGhost(size_type & vertex) const
        {
            return vector_of_vertices[vertex].state == GHOST;
        }

        /* GHOST vertex is invisible for this function */
        size_type DFSfindArticulationPoints(size_type vertex, size_type vertex_parent, std::vector<size_type> & articulation_points, size_type dfs_visit_time = 1)
        {
            bool already_is_articulation_piont = false;                                                                      // to avoid checking each vertex more than once
            size_type child_low;
            vector_of_vertices[vertex].state = VISITED;
            vector_of_vertices[vertex].dfs_id = vector_of_vertices[vertex].low = ++dfs_visit_time;

            for(auto neighbour : vector_of_vertices[vertex].adjacent_vertices)
            {
                if(neighbour != vertex_parent && !vertexIsGhost(neighbour))                                                  // parent and ghost are ignored
                {
                    if(vertexWasNotVisited(neighbour))
                    {
                        child_low = DFSfindArticulationPoints(neighbour, vertex, articulation_points, dfs_visit_time);       // recursive search
                        if(child_low < vector_of_vertices[vertex].low)
                            vector_of_vertices[vertex].low = child_low;
                        if(!already_is_articulation_piont && child_low >= vector_of_vertices[vertex].dfs_id)                 // at least one son without back edge
                        {
                            already_is_articulation_piont = true;
                            articulation_points.push_back(vertex);
                        }
                    }
                    else if(vector_of_vertices[neighbour].dfs_id < vector_of_vertices[vertex].low)                           // if vertex is connected with back edge with neighbour
                        vector_of_vertices[vertex].low = vector_of_vertices[neighbour].dfs_id;
                }
            }
            return vector_of_vertices[vertex].low;
        }


    public:
        Graph(size_type vertices_amount) : num_of_vertices(vertices_amount), vector_of_vertices(vertices_amount)
        {
            for(size_type i = 0; i < vertices_amount; ++i)
                vector_of_vertices[i].vertex_id = i;
        }

        void createEdge(size_type v, size_type u)
        {
            vector_of_vertices[v].setAdjacentTo(u);
            vector_of_vertices[u].setAdjacentTo(v);
        }

        /* for vertex to be an articulation point, one of the following must happen:
         * -> vertex is root of the spanning tree and has more than one child
         *      Uzasadnienie tego faktu jest bardzo proste. Gdy uruchomimy przejście DFS przy tworzeniu drzewa rozpinającego, to dojdzie ono do każdego wierzchołka,
         *      do którego istnieje w grafie ścieżka od wierzchołka startowego. Zatem po uruchomieniu w korzeniu drzewa DFS odwiedzi wszystkich sąsiadów korzenia,
         *      do których w grafie istnieje ścieżka. Jeśli jakiś sąsiad zostanie nieodwiedzony przy powrocie z DFS uruchomionego dla pierwszego z synów, to znaczy,
         *      że w grafie nie było do niego innej ścieżki oprócz krawędzi bezpośrednio od korzenia do tego sąsiada. W takim przypadku powstaje kolejny syn korzenia
         *      (gdyż DFS musimy ponownie uruchomić dla każdego nieodwiedzonego sąsiada). Pomiędzy poprzednim synem istnieje droga tylko poprzez korzeń drzewa
         *      (gdyby istniała inna, to DFS by ją znalazło i dany wierzchołek nie zostałby synem korzenia, lecz jego dalszym potomkiem). Jeśli korzeń zostanie teraz usunięty z grafu,
         *      to wszyscy jego synowie na drzewie rozpinającym znajdą się w oddzielnych spójnych składowych grafu. Liczba tych składowych wzrośnie, zatem korzeń będzie punktem artykulacji.
         * -> vertex isn't root of the spanning tree, but its low >= dfs_id
         *      Istnienie krawędzi wtórnej oznacza, że do syna można dojść również inną drogą, która nie wiedzie poprzez dany wierzchołek(wtedy od syna rozpropaguje sie low < dfs innych).
         *      Skoro tak, to jego usunięcie nie odłączy syna od reszty grafu, gdyż wciąż będzie się znajdował w tej samej spójnej składowej z uwagi na krawędź łączącą jego potomków z innym przodkiem.
         *      Zatem liczba składowych nie wzrośnie. Jeśli natomiast taka krawędź wtórna nie istnieje(low syna == dfs rodzica i zostanie rozpropagowane jako low do rodzica),
         *      to do tego syna można przejść w grafie tylko krawędzią, która łączy go z jego ojcem. Jeśli usuniemy ojca, krawędź zniknie i syn znajdzie się w oddzielnej spójnej składowej.
         *      Liczba składowych grafu wzrośnie, zatem ojciec musi być punktem artykulacji.*/


        std::vector<edge> findWideBridges()
        {
            std::vector<edge> wide_bridges;
            std::vector<size_type> articulation_points;
            size_type num_of_search_started = 0;                                                                       // if > 1 it means that ghost disconnects graph
            size_type root_children;                                                                                   // if > 1 root is an articulation point
            for(size_type ghost_id = 0; ghost_id < num_of_vertices; ghost_id++)                                        // choosing ghost for one iteration
            {
                vector_of_vertices[ghost_id].state = GHOST;
                for(size_type v = 0; v < num_of_vertices; ++v)
                {
                    if(vertexWasNotVisited(v))
                    {
                        ++num_of_search_started;
                        vector_of_vertices[v].state = VISITED;
                        vector_of_vertices[v].dfs_id = 1;                                                              // first visited vertex is root of the spanning tree, its id is always 1
                        root_children = 0;
                        for(auto neighbour : vector_of_vertices[v].adjacent_vertices)
                        {
                            if(vertexWasNotVisited(neighbour))
                            {
                                ++root_children;
                                DFSfindArticulationPoints(neighbour, v, articulation_points);
                            }
                        }

                        if(root_children > 1)
                            articulation_points.push_back(v);
                    }
                }
                if(num_of_search_started > 1)                                                                          // edges containing ghost can be wide bridges
                {
                    for(auto neighbour : vector_of_vertices[ghost_id].adjacent_vertices)
                    {
                        if(vector_of_vertices[neighbour].adjacent_vertices.size() != 1)                                // if neighbour isn't leaf of the spanning tree
                        {
                            bool already_in = false;
                            for(auto existing_bridge : wide_bridges)                                                   // checking if this wide bridge already exists
                            {
                                if((existing_bridge.first == ghost_id && existing_bridge.second == neighbour) ||
                                   (existing_bridge.first == neighbour && existing_bridge.second == ghost_id))
                                    already_in = true;
                            }
                            if(!already_in)
                                wide_bridges.push_back(edge(ghost_id, neighbour));
                        }
                    }
                }
                for(auto a_point : articulation_points)                                                                // for each articulation point
                {
                    for(auto neighbour : vector_of_vertices[ghost_id].adjacent_vertices)                               // looking for ghost neighbours
                    {
                        if(a_point == neighbour)                                                                       // if articulation point is neighbour then it must form with ghost a wide bridge
                        {
                            bool already_in = false;
                            for(auto existing_bridge : wide_bridges)
                            {
                                if((existing_bridge.first == ghost_id && existing_bridge.second == a_point) ||
                                   (existing_bridge.first == a_point && existing_bridge.second == ghost_id))           // checking if this wide bridge already exists
                                    already_in = true;
                            }
                            if(!already_in)
                                wide_bridges.push_back(edge(ghost_id, a_point));
                            break;                                                                                     // if articulation point was on neighbour list, there is no need to continue searching through that list
                        }
                    }
                }
                num_of_search_started = 0;
                articulation_points.clear();                                                                           
                clearAllStates();                                                                                      // reseting states to NOT_VISITED for next iteration
            }
            return wide_bridges;
        }
    };
}// namespace
#endif