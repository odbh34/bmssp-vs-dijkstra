#ifndef BELLMAN_FORD_HPP
#define BELLMAN_FORD_HPP

#include "metrics.hpp"

#include <vector>
#include <limits>
#include <algorithm>

template<typename T>
class BellmanFord {
private:
    struct Edge {
        int from;
        int to;
        T weight;
        Edge(int f, int t, T w) : from(f), to(t), weight(w) {}
    };
    
    int n;  // número de vértices
    std::vector<Edge> edges;  // lista de aristas
    std::vector<T> dist;
    std::vector<int> pred;
    
    static constexpr T INF = std::numeric_limits<T>::max();

public:
    BellmanFord(int vertices) : n(vertices) {}
    
    // Agregar arista dirigida
    void add_edge(int u, int v, T weight) {
        if (u >= 0 && u < n && v >= 0 && v < n) {
            edges.emplace_back(u, v, weight);
        }
    }
    
    // Ejecutar Bellman-Ford desde un nodo fuente
    std::pair<std::vector<T>, std::vector<int>> execute(int source, Metrics* M = nullptr) {
        if (source < 0 || source >= n) {
            return {{}, {}};
        }

        dist.assign(n, INF);
        pred.assign(n, -1);
        if (M) M->assign(2 * n);   // inicialización de vectores

        dist[source] = 0;
        if (M) M->assign();

        // Relajación de aristas: V-1 iteraciones
        for (int i = 0; i < n - 1; ++i) {
            if (M) M->compare();
            
            bool relaxed = false;
            if (M) M->assign();
            
            for (const auto& edge : edges) {
                if (M) {
                    M->access(3);  // edge.from, edge.to, edge.weight
                }
                
                // Solo relajar si el vértice origen ya tiene distancia conocida
                if (dist[edge.from] != INF) {
                    if (M) {
                        M->compare();
                        M->access();
                    }
                    
                    T new_dist = dist[edge.from] + edge.weight;
                    if (M) {
                        M->arithmetic();
                        M->assign();
                    }
                    
                    if (new_dist < dist[edge.to]) {
                        if (M) {
                            M->compare();
                            M->access(2);
                            M->assign(2);
                        }
                        
                        dist[edge.to] = new_dist;
                        pred[edge.to] = edge.from;
                        relaxed = true;
                    }
                }
            }
            
            // Optimización: si no hubo relajaciones, terminamos antes
            if (!relaxed) {
                if (M) M->compare();
                break;
            }
        }

        return {dist, pred};
    }
    
    // Reconstruir el camino más corto hasta un destino
    std::vector<int> get_shortest_path(int destination) const {
        if (destination < 0 || destination >= n || pred.empty()) {
            return {};
        }
        
        if (dist[destination] == INF) {
            return {};  // No hay camino
        }
        
        std::vector<int> path;
        int current = destination;
        
        while (current != -1) {
            path.push_back(current);
            current = pred[current];
        }
        
        std::reverse(path.begin(), path.end());
        return path;
    }
    
    // Verificar si hay camino a un destino
    bool has_path(int destination) const {
        return destination >= 0 && destination < n && 
               !dist.empty() && dist[destination] != INF;
    }
    
    // Obtener distancia a un destino
    T get_distance(int destination) const {
        if (destination >= 0 && destination < n && !dist.empty()) {
            return dist[destination];
        }
        return INF;
    }
    
    // Detectar ciclos negativos alcanzables desde source
    bool has_negative_cycle(int source) const {
        if (source < 0 || source >= n || dist.empty()) {
            return false;
        }
        
        // Ejecutar una iteración adicional
        std::vector<T> test_dist = dist;
        
        for (const auto& edge : edges) {
            if (test_dist[edge.from] != INF) {
                T new_dist = test_dist[edge.from] + edge.weight;
                if (new_dist < test_dist[edge.to]) {
                    return true;  // Ciclo negativo detectado
                }
            }
        }
        
        return false;
    }
    
    // Limpiar el grafo
    void clear() {
        edges.clear();
        dist.clear();
        pred.clear();
    }
    
    // Getters
    int get_vertices() const { return n; }
    int get_edges_count() const { return edges.size(); }
    const std::vector<T>& get_distances() const { return dist; }
    const std::vector<int>& get_predecessors() const { return pred; }
    
    // Valor infinito público para comparaciones
    static T infinity() { return INF; }
};

#endif // BELLMAN_FORD_HPP

